AUTOMAKE_OPTIONS = gnu

lib_LTLIBRARIES = libopentracing.la

libopentracing_la_SOURCES = opentracing/span.cc opentracing/tracer.cc

nobase_include_HEADERS = opentracing/span.h opentracing/tracer.h

libopentracing_la_LDFLAGS = -version-info 1:0:0


ACLOCAL_AMFLAGS = -I m4

AM_CXXFLAGS=  -Wall -fno-elide-constructors -pedantic-errors -ansi -Wextra -Wall
AM_CXXFLAGS+= -Winit-self -Wold-style-cast -Woverloaded-virtual -Winit-self
AM_CXXFLAGS+= -Wuninitialized -Wmissing-declarations -Werror -std=c++98
