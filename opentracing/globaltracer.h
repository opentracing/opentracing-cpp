#ifndef INCLUDED_OPENTRACING_GLOBALTRACER_H
#define INCLUDED_OPENTRACING_GLOBALTRACER_H

// Only if the singleton interface is included do we require that opentracing
// produce a .a or .so library. To avoid building a physical lib for a 'flea on
// the elephant', I've moved the singleton GlobalTracer struct. It's only
// built if requested.

namespace opentracing {

class Tracer;

struct GlobalTracer
{
    public:
      static void install(Tracer*);
      static void uninstall();
      static Tracer* instance();

    private:
      static GlobalTracer* s_tracer;
};

} // namespace opentracing
// #INCLUDED_OPENTRACING_GLOBALTRACER_H
