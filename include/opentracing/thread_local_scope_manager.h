#ifndef OPENTRACING_THREAD_LOCAL_SCOPE_MANAGER_H
#define OPENTRACING_THREAD_LOCAL_SCOPE_MANAGER_H

#include <opentracing/scope_manager.h>
#include <opentracing/version.h>

#include <memory>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE

class Span;

// A ScopeManager for propagating Spans within the same thread.
//
// Once activated, during the lifetime of the Scope, the Span can be
// accessed only within the same thread. This behaviour is best for
// propagating Spans down the execution stack without requiring each
// component to forward it explicitly. This is achieved using
// thread_local variables.
class ThreadLocalScopeManager : public ScopeManager {
 public:
  // Activate the given Span, returning a Scope to track its duration.
  //
  // An activated Span is only accessible from the same thread as the
  // Activation for the lifetime of the Scope. The Scope MUST be stored
  // on the Stack and MUST NOT be moved beyond the time of
  // instantiation, otherwise behaviour is undefined.
  Scope Activate(std::shared_ptr<Span> span) noexcept override;

  // Return the current active Span.
  //
  // A span is always guaranteed to be returned. If there is no span
  // active, then a default noop span instance should be returned.
  // Only active Spans from the same thread as the activation are
  // returned.
  std::shared_ptr<Span> ActiveSpan() noexcept override;
};

END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_THREAD_LOCAL_SCOPE_MANAGER_H
