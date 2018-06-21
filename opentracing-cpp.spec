Name:           %{_name}
Summary:        %{_summary}
Version:        %{_version}
Release:        %{_release}
License:        Apache License 2.0
Url:            %{_url}

%description
opentracing .so files

%prep

%build

%install
mkdir -p ${RPM_BUILD_ROOT}/usr/lib64/
cp -f %{_binary_dir}/output/libopentracing.so.%{_version_num} $RPM_BUILD_ROOT/usr/lib64/
cp -f %{_binary_dir}/output/libopentracing_mocktracer.so.%{_version_num} $RPM_BUILD_ROOT/usr/lib64/

%post
# post-install
# Run ldconfig to generate linker names
/sbin/ldconfig

%postun
# post-uninstall
# Run ldconfig to remove linker names
/sbin/ldconfig

%files
/usr/lib64/libopentracing.so.%{_version_num}
/usr/lib64/libopentracing_mocktracer.so.%{_version_num}
