#include <opentracing/noop.h>
#include <opentracing/scope_manager.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace {

thread_local Span* active = nullptr;

}  // anonymous namespace

Scope::Scope(Span& span) noexcept {}

Scope::~Scope() noexcept {}

Scope ScopeManager::Activate(Span& span) noexcept { return {span}; }

Span& ScopeManager::ActiveSpan() noexcept { return *active; }

END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
