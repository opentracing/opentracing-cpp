#ifndef OPENTRACING_SCOPE_MANAGER_H
#define OPENTRACING_SCOPE_MANAGER_H

#include <opentracing/version.h>
#include <functional>
#include <memory>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE

class Span;
class ScopeManager;

// Scope is returned by the ScopeManager when activating a span.
//
// The lifetime of the Scope instance represents the duration of the
// activation. Its lifetime can not exist beyond that of the
// ScopeManager.
class Scope {
 public:
  using Callback = std::function<void()>;

  // Create a Scope that will invoke callback on destruction.
  Scope(Callback callback) noexcept;

  Scope(Scope&& scope) noexcept;
  ~Scope();

 private:
  Scope(const Scope& scope) = delete;
  Scope& operator=(const Scope&) = delete;
  Scope& operator=(Scope&&) = delete;

  Callback callback_;
};

// ScopeManager allows a Span to be activated for a specific scope.
//
// Once a Span has been activated, it can then be accessed via the
// ScopeManager. This interface can be implemented to provide
// different characteristics of Span propagation such as passing
// only within the same thread.
class ScopeManager {
 public:
  virtual ~ScopeManager() = default;

  // Activate the given Span, returning a Scope to track its duration.
  //
  // A Span MUST be upgraded to a shared_ptr as consumers of the span
  // via the ScopeManager may take ownership over it beyond the
  // duration of the Scope. Implementations are expected to define the
  // logic of Scope destrucion.
  virtual Scope Activate(std::shared_ptr<Span> span) noexcept = 0;

  // Return the current active Span.
  //
  // A span is always guaranteed to be returned. If there is no span
  // active, then a default noop span instance should be returned.
  virtual std::shared_ptr<Span> ActiveSpan() noexcept = 0;
};

END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_SCOPE_MANAGER_H
