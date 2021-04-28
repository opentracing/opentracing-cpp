#include <opentracing/noop.h>
#include <opentracing/scope_manager.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE

Scope::Scope(Callback callback) noexcept : callback_(callback) {}

Scope::Scope(Scope&& scope) noexcept { std::swap(callback_, scope.callback_); }

Scope::~Scope() {
  if (callback_) {
    callback_();
  }
}

END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
