#ifndef OPENTRACING_SPAN_H
#define OPENTRACING_SPAN_H

#include <string>
#include <vector>
#include <sstream>
#include <stdint.h>

namespace opentracing
{

class Tracer;
class LogData;

/**
 * A `Span` represents an active, un-finished span in the OpenTracing system.
 * Spans are created by the `Tracer` interface.
 */
class Span
{
public:
	/**
	 * Destroys this Span.
	 */
    virtual ~Span();

    /**
     * Sets or changes the operation name.
     */
    virtual Span * setOperationName(const std::string & operationName);
    
    /**
     * Adds a tag with any value to the `Span`. If there is a
     * existing tag set for `key`, it is overwritten.
     */
    template <typename T>
    Span * setTag(const std::string & key, const T & value);

    /**
	 * Adds a tag with a `std::string` value to the `Span`. If there is a
	 * existing tag set for `key`, it is overwritten.
	 */
    virtual Span * setTag(const std::string & key, const std::string & value);

    /**
     * Log() records `data` to this Span.
     * See LogData for semantic details.
     */
    virtual Span * log(const LogData & data);

    /**
     * Bulk, potentially more efficient version of log().
     */
    virtual Span * log(const std::vector<LogData> & logData);

    /**
     * `setBaggageItem()` sets a key:value pair on this `Span` that also
     * propagates to future `Span` children.
     *
     * `setBaggageItem()` enables powerful functionality given a full-stack
     * opentracing integration (e.g., arbitrary application data from a mobile
     * app can make it, transparently, all the way into the depths of a storage
     * system), and with it some powerful costs: use this feature with care.
     *
     * IMPORTANT NOTE #1: `setBaggageItem()` will only propagate trace
     * baggage items to *future* children of the `Span`.
     *
     * IMPORTANT NOTE #2: Use this thoughtfully and with care. Every key and
     * value is copied into every local *and remote* child of this `Span`, and
     * that can add up to a lot of network and CPU overhead.
     *
     * IMPORTANT NOTE #3: Baggage item keys have a restricted format:
     * implementations may wish to use them as HTTP header keys (or key
     * suffixes), and of course HTTP headers are case insensitive.
     *
     * As such, `restrictedKey` MUST match the regular expression
     * `(?i:[a-z0-9][-a-z0-9]*)` and is case-insensitive. That is, it must
     * start with a letter or number, and the remaining characters must be
     * letters, numbers, or hyphens. See `canonicalizeBaggageKey()`. If
     * `restrictedKey` does not meet these criteria, `setBaggageItem()`
     * results in undefined behavior.
     *
     * Returns a pointer to this `Span` for chaining, etc.
     */
    virtual Span * setBaggageItem(const std::string & restrictedKey, const std::string & value);

    /*
     * If `targetValue` is `NULL`, this method quickly returns true/false depending on
     * if baggage is set for given key or not. If `targetValue` isn't `NULL` then the
     * baggage value is assigned to `*targetValue` if it exists, otherwise `*targetValue`
     * doesn't get modified.
     *
     * See the `setBaggageItem()` notes about `restrictedKey`.
     */
    virtual bool getBaggageItem(const std::string & restrictedKey, std::string * targetValue = 0) const;

    /**
     * Provides access to the `Tracer` that created this `Span`. Implementation should
     * ensure the reference remains valid potentially in a multi-threaded context (might
     * require some form of reference counting).
     */
    virtual const Tracer & getTracer() const = 0;

    /**
     * Prints minimal information able to identify the span (e.g. trace id/span id) to an
     * output stream. This is useful in order to be able to corellate traces with other
     * types of logs.
     */
    virtual std::ostream & print(std::ostream & os) const = 0;

    /**
     * Stops this span. `finish()` should be the last call made to any span instance,
     * and to do otherwise leads to undefined behavior.
     */
    virtual void finish();

    /**
     * Same as `finish()` above but also sets explicitly the finishing time
     * in microseconds since epoch. If set to the default value (the unix
	 * epoch), implementations should use the current time implicitly.
     */
    virtual void finish(const uint64_t & finishTime);
};

/**
 * LogData is data associated to a Span. Every LogData instance should specify
 * at least one of Event and/or Payload.
 */
class LogData
{
public:
	/**
	 * Payload should support arbitrary binary data. `std::string` can support
	 * this.
	 */
	typedef std::string Payload;

	/**
	 * Creates an empty `LogData` for current time and with optional parameters.
	 */
	explicit LogData(const std::string & event = "", const Payload & payload = Payload());

	/**
	 * Creates an empty `LogData` for the given timestamp and with optional
	 * parameters. If set to the default value (the unix epoch), implementations
	 * should use the current time implicitly.
	 */
	explicit LogData(const uint64_t & timestamp, const std::string & event = "", const Payload & payload = Payload());

	/**
	 * The timestamp of the log record; if set to the default value (the unix
	 * epoch), implementations should use the current time implicitly.
	 */
    uint64_t timestamp;

    /**
     * Event (if non-empty) should be the stable name of some notable moment in
     * the lifetime of a `Span`. For instance, a `Span` representing a browser page
     * load might add an Event for each of the Performance.timing moments
     * here: https://developer.mozilla.org/en-US/docs/Web/API/PerformanceTiming
     *
     * While it is not a formal requirement, Event strings will be most useful
     * if they are *not* unique; rather, tracing systems should be able to use
     * them to understand how two similar `Span`s relate from an internal timing
     * perspective.
     */
    std::string event;

	/**
	 * Payload is a free-form potentially structured object which Tracer
	 * implementations may retain and record all, none, or part of.
	 *
	 * If included, `payload` should be restricted to data derived from the
	 * instrumented application; in particular, it should not be used to pass
	 * semantic flags to a log() implementation.
	 *
	 * For example, an RPC system could log the wire contents in both
	 * directions, or a SQL library could log the query (with or without
	 * parameter bindings); tracing implementations may truncate or otherwise
	 * record only a snippet of these payloads (or may strip out PII, etc,
	 * etc).
	 */
    Payload payload;
};

/**
 * `canonicalizeBaggageKey()` returns the canonicalized version of baggage item
 * key `key`, and true if and only if the key was valid. Note: It might alter
 * `key` irrespectively of if it is valid or not.
 */
bool canonicalizeBaggageKey(std::string & key);

/**
 * Default `setTag()` implementation.
 */
template <typename T>
Span * Span::setTag(const std::string & key, const T & value)
{
    std::ostringstream oss;
    oss << value;
    setTag(key, oss.str());
    return this;
}

}

#endif // #define OPENTRACING_SPAN_H
