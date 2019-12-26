#ifndef OPENTRACING_SCOPE_MANAGER_H
#define OPENTRACING_SCOPE_MANAGER_H

#include <opentracing/version.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE

class Span;

// Scope is returned by the ScopeManager when activating a span
//
// The lifetime of the Scope instance represends the duration of the
// activation. A Scope cannot be created and can only be returned by
// the ScopeManager.
class Scope {
 private:
  // Create an activation Scope for the given Span.
  Scope(Span& span) noexcept;
  ~Scope() noexcept;

  Scope(const Scope&) = delete;
  Scope(Scope&&) = delete;
  Scope& operator=(const Scope&) = delete;
  Scope& operator=(Scope&&) = delete;

  friend class ScopeManager;
};

// ScopeManager allows a Span to be activated for a specific scope.
//
// Once a span has been activated, it can then be accessed by any code
// executed within the lifetime of the scope and the same thread only.
class ScopeManager {
 public:
  // Activate the given Span, returning a Scope to track its duration.
  static Scope Activate(Span& span) noexcept;

  // Return a reference to the current active Span.
  //
  // A span is always guaranteed to be returned. If there is no span
  // active, then a default noop span instance will be returned.
  static Span& ActiveSpan() noexcept;
};

END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_SCOPE_MANAGER_H
