#include <opentracing/noop.h>
#include <opentracing/thread_local_scope_manager.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE

Scope ThreadLocalScopeManager::Activate(std::shared_ptr<Span> span) noexcept {}

std::shared_ptr<Span> ThreadLocalScopeManager::ActiveSpan() noexcept {}

END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
