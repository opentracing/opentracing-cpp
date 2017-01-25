#include <opentracing/globaltracer.h>
#include <opentracing/tracer.h>

namespace opentracing{

void GlobalTracer::install(Tracer *)
{
    s_tracer = tracer;
}

void GlobalTracer::uninstall()
{
    s_tracer = 0;
}

Tracer* GlobalTracer::instance()
{
    return s_Tracer;
}

}
