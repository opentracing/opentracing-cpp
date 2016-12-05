# OpenTracing API for C++
C++ implementation of the OpenTracing API http://opentracing.io

[![Join the chat at https://gitter.im/opentracing/opentracing-cpp](https://badges.gitter.im/opentracing/opentracing-cpp.svg)](https://gitter.im/opentracing/opentracing-cpp?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

## Required Reading

In order to understand the C++ platform API, one must first be familiar with the
[OpenTracing project](http://opentracing.io) and
[terminology](http://opentracing.io/spec/) more generally. This is a C++98 API that
is used as a "common denominator". Ît's up to implementors to choose the C++ level
they are going to use for their implementations:

![stack of libraries](img/stack-of-libraries.png "Stack of Libraries")

## Compile and install

```
libtoolize # or glibtoolize
./autogen.sh
./configure
sudo make install
```

To test (requires gtest - see [here for OS X](http://stackoverflow.com/questions/20746232/how-to-properly-setup-googletest-on-os-x-aside-from-xcode), [here for ubuntu](http://www.eriksmistad.no/getting-started-with-google-test-on-ubuntu/) and [here for Red Hat](http://stackoverflow.com/questions/13513905/how-to-setup-googletest-as-a-shared-library-on-linux)/for Red Hat note also [this](http://stackoverflow.com/questions/4743233/is-usr-local-lib-searched-for-shared-libraries)):

```
cd test
make
./test
```

Note - You might have to set your `LD_LIBRARY_PATH`:

```
LD_LIBRARY_PATH=/usr/local/lib ./test
```

## API overview for those adding instrumentation

Everyday consumers of this `opentracing` package really only need to worry
about a couple of key abstractions: the `StartSpan` function, the `Span`
interface, and binding a `Tracer` at `main()`-time.
