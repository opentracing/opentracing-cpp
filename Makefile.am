AUTOMAKE_OPTIONS = gnu

lib_LTLIBRARIES =

nobase_include_HEADERS = opentracing/*.h opentracing/internal/*.h

ACLOCAL_AMFLAGS = -I m4

AM_CXXFLAGS= -fno-elide-constructors -pedantic-errors -ansi 

if COMPILER_IS_GCC
AM_CXXFLAGS+= -Wall -Werror -Wextra -Winit-self -Wold-style-cast
AM_CXXFLAGS+= -Woverloaded-virtual -Wuninitialized -Wmissing-declarations
endif
