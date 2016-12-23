AUTOMAKE_OPTIONS = gnu

lib_LTLIBRARIES = libopentracing.la

libopentracing_la_SOURCES = opentracing/span.cc opentracing/tracer.cc

nobase_include_HEADERS = opentracing/span.h opentracing/tracer.h

libopentracing_la_LDFLAGS = -version-info 1:0:0


ACLOCAL_AMFLAGS = -I m4

AM_CXXFLAGS=  -fno-elide-constructors -pedantic-errors -ansi -std=c++98

if COMPILER_IS_GCC
AM_CXXFLAGS+= -Wall -Werror -Wextra -Winit-self -Wold-style-cast 
AM_CXXFLAGS+= -Woverloaded-virtual -Wuninitialized -Wmissing-declarations 
endif
