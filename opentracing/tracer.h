#ifndef OPENTRACING_TRACER_H
#define OPENTRACING_TRACER_H

#include <string>
#include <stdint.h>

namespace opentracing
{

class Span;
class Writer;
class Reader;

/**
 * Options used when you start a `Span`.
 */
struct StartSpanOptions
{
    /**
     * The parent span. `NULL` if there's no parent `Span`.
     */
    const Span * parent;

    /**
     * The `Span` start time. Set to 0 to use current time.
     */
    const uint64_t startTime;
    
    /**
     * Start a child `Span` with the given parent and `startTime`.
     */
    StartSpanOptions(const Span * parent_, const uint64_t & startTime_ = 0);
    
    /**
     * Start a child `Span` with the given `startTime`.
     */
    explicit StartSpanOptions(const uint64_t & startTime_ = 0);
};

/**
 * Tracer is a simple, thin interface for `Span` creation.
 */
class Tracer
{
public:
    /**
     * Destroys the `Tracer`.
     */
    virtual ~Tracer();

    /**
     * Return statuses for the `inject()` and `join()` methods.
     */
    enum Result
    {        
        /**
         * Operation completed successfully.
         */
        Success,

        /**
         * `ErrUnsupportedFormat` occurs when the `format` passed to `Tracer::inject()` or
         * `Tracer::join()` is not recognized by the Tracer implementation.
         */
        ErrUnsupportedFormat,

        /**
         * `ErrTraceNotFound` occurs when the `carrier` passed to `Tracer::join()` is
         * valid and uncorrupted but has insufficient information to join or resume
         * a trace.
         */
        ErrTraceNotFound,

        /**
         * `ErrInvalidSpan` errors occur when `Tracer::inject()` is asked to operate on
         * a `Span` which it is not prepared to handle (for example, since it was
         * created by a different tracer implementation).
         */
        ErrInvalidSpan,

        /**
         * `ErrInvalidCarrier` errors occur when `Tracer::inject()` or `Tracer::join()`
         * implementations expect a different type of `carrier` than they are
         * given.
         */
        ErrInvalidCarrier,

        /**
         * `ErrTraceCorrupted` occurs when the `carrier` passed to `Tracer::join()` is
         * of the expected type but is corrupted.
         */
        ErrTraceCorrupted
    };

    /**
     * Create, start, and return a new Span with the given `operationName`, all
     * without specifying a parent `Span` that can be used to incorporate the
     * newly-returned `Span` into an existing trace. (I.e., the returned `Span` is
     * the "root" of its trace).
     *
     * Examples:
     *
     *     Span * sp = globalTracer()->startSpan("GetFeed()");
     *
     *     Span * sp = globalTracer()->startSpan("LoggedHTTPRequest()", StartSpanOptions(now, &parent))
     *                               ->setTag("user_agent", loggedReq.UserAgent);
     *
     * This method isn't expected to fail except of the usual out of memory errors.
     */
    virtual Span * startSpan(const std::string & operationName, const StartSpanOptions & startSpanOptions = StartSpanOptions()) const = 0;

    /**
     * `inject()` takes the `sp` `Span` instance and sends information topropagation
     * carriers via the `Writer` interface. It returns `Result::Success` on success
     * or some error otherwise. In the latter case, `error` will be set with a
     * debug message.
     *
     * Example usage:
     *
     *      CarrierT carrier;
     *      std::string error;
     *
     *      Tracer::Result result = globalTracer()->inject(*span,
     *                                                     MapTextAdapter<CarrierT>(carrier),
     *                                                     error);
     *      if (result != Tracer::Success) { ...
     *
     * NOTE: All opentracing.Tracer implementations MUST support `TextMapWriter`
     * `Writer`s.
     *
     * Implementations may return `Result::ErrUnsupportedFormat` if `writer`
     * is of unknown or unsupported type.
     *
     * Implementations may return `Result::ErrInvalidCarrier` or any other
     * implementation-specific error if the format is supported but injection
     * fails anyway.
     *
     * See also `Tracer::join()`.
     */
    virtual Result inject(const Span & sp, const Writer & writer, std::string & error) const = 0;

    /**
     * `join()` returns a `Span` instance with operation name `operationName` given
     * a carrier reader `reader`.
     *
     * `join()` is responsible for extracting and joining to the trace of a `Span`
     * instance embedded in a format-specific "carrier" object. Typically the
     * joining will take place on the server side of an RPC boundary, but
     * message queues and other IPC mechanisms are also reasonable places to
     * use `join()`.
     *
     * OpenTracing defines an extensible set of `Reader`s (see `Reader`), and
     * each supports potentially many carrier types.
     *
     *
     *
     * Example usage:
     *
     *      CarrierT carrier;
     *      std::string error;
     *
     *      Span * span(0);
     *      Tracer::Result result = globalTracer()->join(&span,
     *                                                   "operation()",
     *                                                   MapTextAdapter<CarrierT>(carrier),
     *                                                   error);
     *      if (result != Tracer::Success) { ...
     *
     * NOTE: All `Tracer` implementations MUST support `TextMapReader` `Reader`s.
     *
     * Return values:
     *  - A successful `join()` will return `Tracer::Success` and will set `sp` to
     *    the value of an appropriately initialized `Span` pointer.
     *  - If there was simply no trace to join with in `carrier`, `join()`
     *    returns `Tracer::ErrTraceNotFound` and `sp` isn't modified.
     *  - If the `reader` is unsupported or unrecognized, `join()` returns
     *    `Tracer::ErrUnsupportedFormat` and `sp` isn't modified.
     *  - If there are more fundamental problems with the `reader` object,
     *    `join()` may return `Tracer::ErrInvalidCarrier` or
     *    `Tracer::ErrTraceCorrupted` and `sp` won't be modified.
     *
     *  See also `Tracer::inject()`.
     */
    virtual Result join(Span ** sp, const std::string & operationName, const Reader & reader, std::string & error) const = 0;
};

/**
 * This is the base interface for writing data to carriers. Not much
 * functionality here but provides through `dynamic_cast<>`ing the means
 * to check if an implementation is supported or not.
 */
class Writer
{
public:
    /**
     * Destroys this Writer.
     */
    virtual ~Writer();
};

/**
 * This is the base interface for reading data from carriers. Not much
 * functionality here but provides through `dynamic_cast<>`ing the means
 * to check if an implementation is supported or not.
 */
class Reader
{
public:
    /**
     * Destroys this Reader.
     */
    virtual ~Reader();
};

/**
 * This is an implementation of `Writer` which should be supported
 * by every tracing system. It allows the propagation of string key-value
 * pairs through carriers.
 */
class TextMapWriter : public virtual Writer
{
public:
    /**
     * Destroys this TextMapWriter.
     */
    virtual ~TextMapWriter();

    /**
     * Sets a key-value pair to the carrier. `isBaggage` will be true for baggage key-values.
     */
    virtual void set(const std::string& key, const std::string& value, bool isBaggage) const = 0;
};

/**
 * This is an implementation of `Reader` which should be supported
 * by every tracing system. It allows joining Spans by using information
 * on the form of string key-value pairs from the carriers.
 */
class TextMapReader : public virtual Reader
{
public:
    /**
     * Destroys this TextMapWriter.
     */
    virtual ~TextMapReader();

    /**
     * The type of callback which will be called for each key.
     */
    struct ReadCallback
    {
        virtual ~ReadCallback();
        virtual void operator() (const std::string& key, const std::string& value, bool isBaggage) = 0;
    };

    /**
     * Reads from the carrier and calls the `callback()` for each key-value pair.
     */
    virtual void forEachPair(const ReadCallback & callback) const = 0;
};

/**
 * Default No-op implementation. Doesn't crash nor has any side-effects.
 */
struct NoopTracer : public Tracer
{
    /**
     * Destroys this NoopTracer.
     */
    virtual ~NoopTracer();

    /**
     * Starts a `Span` for a given operation name and options defined in
     * `startSpanOptions`.
     */
    virtual Span * startSpan(const std::string & operationName, const StartSpanOptions & startSpanOptions = StartSpanOptions()) const;

    /**
     * The no-op Tracer's `inject()` method should always succeed
     */
    virtual Result inject(const Span & sp, const Writer & writer, std::string & error) const;


    /**
     * The no-op Tracer's `join()` method should always return a "trace not found"
     * status.
     */
    virtual Result join(Span ** sp, const std::string & operationName, const Reader & reader, std::string & error) const;
};

/**
 * Returns the previous instance of the global tracer or `NULL` if
 * the previous version was the default no-op `Tracer`. This method isn't
 * thread safe by default. To make it thread-safe define
 * `OPENTRACING_USE_STD_ATOMIC` to use `std::atomic<>` or
 * `OPENTRACING_USE_BOOST_ATOMIC` to use `boost::atomic<>`. This function
 * throws `std::runtime_error` if the `tracer` argument is `NULL`.
 */
Tracer * initGlobalTracer(Tracer * tracer);

/**
 * Returns the global tracer object. It's guaranteed to not be `NULL`. The
 * same thread safety rules as for `initGlobalTracer()` apply.
 */
Tracer * globalTracer();

}

#endif // #ifndef OPENTRACING_TRACER_H
