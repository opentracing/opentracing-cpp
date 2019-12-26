#include <opentracing/noop.h>
#include <opentracing/scope_manager.h>

using namespace opentracing;

#define CATCH_CONFIG_MAIN
#include <opentracing/catch2/catch.hpp>

class MockScopeManager : public ScopeManager {
 public:
  MockScopeManager()
      : default_span_(MakeNoopTracer()->StartSpan("")), set_span_() {}

  Scope Activate(std::shared_ptr<Span> span) noexcept override {
    return Scope(*this, span);
  }

  std::shared_ptr<Span> ActiveSpan() noexcept override { return default_span_; }

  std::shared_ptr<Span> default_span_;
  std::shared_ptr<Span> set_span_;

 private:
  void SetActiveSpan(std::shared_ptr<Span> span) noexcept override {
    set_span_ = span;
  }
};

TEST_CASE("scope") {
  MockScopeManager manager;
  auto tracer = MakeNoopTracer();
  auto span = std::shared_ptr<Span>{tracer->StartSpan("a")};

  // Validate that the test mock is sane
  CHECK(manager.ActiveSpan() == manager.default_span_);
  CHECK(manager.set_span_ == nullptr);

  SECTION("Check that Scope calls SetActiveSpan correctly.") {
    {
      auto Scope = manager.Activate(span);
      CHECK(manager.set_span_ == span);
    }
    CHECK(manager.set_span_ == manager.default_span_);
  }
}
