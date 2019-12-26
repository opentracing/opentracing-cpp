#ifndef OPENTRACING_SCOPE_MANAGER_H
#define OPENTRACING_SCOPE_MANAGER_H

#include <opentracing/version.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE

class Span;
class ScopeManager;

// Scope is returned by the ScopeManager when activating a span
//
// The lifetime of the Scope instance represends the duration of the
// activation. A Scope will be returned when Activate is called on the
// the ScopeManager. Its lifetime can not exist beyond that of the
// ScopeManager.
class Scope {
 public:
  Scope(ScopeManager& manager, std::shared_ptr<Span> span);
  Scope(Scope&& scope) noexcept;
  ~Scope();

 private:
  Scope(const Scope& scope) = delete;
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
  virtual ~ScopeManager() = default;

  // Activate the given Span, returning a Scope to track its duration.
  //
  // A Span MUST be upgraded to a shared_ptr as consumers of the span
  // via the ScopeManager may take ownership over it beyond the
  // duration of the Scope.
  virtual Scope Activate(std::shared_ptr<Span> span) noexcept = 0;

  // Return a reference to the current active Span.
  //
  // A span is always guaranteed to be returned. If there is no span
  // active, then a default noop span instance will be returned.
  virtual std::shared_ptr<Span> ActiveSpan() noexcept = 0;

 private:
  // Set the active Span, used by the Scope class.
  virtual void SetActiveSpan(std::shared_ptr<Span> span) noexcept = 0;

  friend class Scope;
};

END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_SCOPE_MANAGER_H
