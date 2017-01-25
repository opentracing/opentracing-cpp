#include <app_tracing.h>

#include <memory>

/**
 * To run:
 * clear && g++ -I. -D__USE_XOPEN2K8 main.cpp build/opentracing/libopentracing.a -o main && ./main
 */
int main()
{
    // Setup
    ZipkinV1 t1;
    ZipkinV2 t2;

    MyHotTracer imp(t1, t2);

    GlobalTracer::install(&imp);


    {
        // Application code
        using Tc = Cpp11GlobalTracer;

        Tc tracer = Tc::instance();

        std::shared_ptr<Tc::Span> span = tracer.start("hi");

        std::vector<opentracing::TextMapPair> pairs;

        tracer.inject(&pairs, *span);

        std::shared_ptr<const Tc::SpanContext> ctx = tracer.extract(pairs);
    }

    // Cleanup
    GlobalTracer::uninstall();

    return 0;
}
