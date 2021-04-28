#include <opentracing/noop.h>
#include <opentracing/thread_local_scope_manager.h>

#include <map>
#include <utility>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE

namespace {

const std::shared_ptr<Span> noopspan{MakeNoopTracer()->StartSpan("")};
using ThreadLocalSpanMap =
    std::map<ThreadLocalScopeManager*, std::shared_ptr<Span>>;
thread_local ThreadLocalSpanMap thread_local_span_map;

}  // anonymous namespace

Scope ThreadLocalScopeManager::Activate(std::shared_ptr<Span> span) noexcept {
  auto it_bool = thread_local_span_map.insert(std::make_pair(this, span));
  if (it_bool.second) {
    return Scope{[this]() { thread_local_span_map.erase(this); }};
  } else {
    std::swap(it_bool.first->second, span);
    return Scope{[this, span]() { thread_local_span_map[this] = span; }};
  }
}

std::shared_ptr<Span> ThreadLocalScopeManager::ActiveSpan() noexcept {
  auto active_it = thread_local_span_map.find(this);
  if (active_it != thread_local_span_map.end()) {
    return active_it->second;
  }
  return noopspan;
}

END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
