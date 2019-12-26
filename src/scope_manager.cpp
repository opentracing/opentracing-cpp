#include <opentracing/noop.h>
#include <opentracing/scope_manager.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE

Scope::Scope(ScopeManager& manager, std::shared_ptr<Span> span) {}

Scope::Scope(Scope&& scope) noexcept {}

Scope::~Scope() {}

END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
