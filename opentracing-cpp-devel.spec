Name:           opentracing-cpp-devel
Summary:        opentracing headers
Version:        1.0
Release:        1
License:        Apache License 2.0
URL:            https://github.com/rnburn/opentracing-cpp

%description
header files for opentracing

%prep

%build

%install
mkdir -p ${RPM_BUILD_ROOT}/usr/include/opentracing/expected
mkdir -p ${RPM_BUILD_ROOT}/usr/include/opentracing/variant
cp -f %{_origdir}/include/opentracing/noop.h $RPM_BUILD_ROOT/usr/include/opentracing
cp -f %{_origdir}/include/opentracing/propagation.h $RPM_BUILD_ROOT/usr/include/opentracing
cp -f %{_origdir}/include/opentracing/span.h $RPM_BUILD_ROOT/usr/include/opentracing
cp -f %{_origdir}/include/opentracing/string_view.h $RPM_BUILD_ROOT/usr/include/opentracing
cp -f %{_origdir}/include/opentracing/symbols.h $RPM_BUILD_ROOT/usr/include/opentracing
cp -f %{_origdir}/include/opentracing/tracer.h $RPM_BUILD_ROOT/usr/include/opentracing
cp -f %{_origdir}/include/opentracing/util.h $RPM_BUILD_ROOT/usr/include/opentracing
cp -f %{_origdir}/include/opentracing/value.h $RPM_BUILD_ROOT/usr/include/opentracing
cp -f %{_origdir}/.build/include/opentracing/config.h $RPM_BUILD_ROOT/usr/include/opentracing
cp -f %{_origdir}/.build/include/opentracing/version.h $RPM_BUILD_ROOT/usr/include/opentracing
cp -f %{_origdir}/3rd_party/include/opentracing/expected/expected.hpp $RPM_BUILD_ROOT/usr/include/opentracing/expected
cp -f %{_origdir}/3rd_party/include/opentracing/variant/variant.hpp $RPM_BUILD_ROOT/usr/include/opentracing/variant
cp -f %{_origdir}/3rd_party/include/opentracing/variant/recursive_wrapper.hpp $RPM_BUILD_ROOT/usr/include/opentracing/variant


%files
/usr/include/opentracing/noop.h
/usr/include/opentracing/propagation.h
/usr/include/opentracing/span.h
/usr/include/opentracing/string_view.h
/usr/include/opentracing/symbols.h
/usr/include/opentracing/tracer.h
/usr/include/opentracing/util.h
/usr/include/opentracing/config.h
/usr/include/opentracing/value.h
/usr/include/opentracing/version.h
/usr/include/opentracing/expected/expected.hpp
/usr/include/opentracing/variant/variant.hpp
/usr/include/opentracing/variant/recursive_wrapper.hpp
