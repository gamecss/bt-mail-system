Summary: Courier Unicode Library
%if 0%{?compat:1}
Name: courier-unicode%(echo 2.2.3 | tr -d '.')

%define __brp_ldconfig %{nil}

%else
Name: courier-unicode
%endif
Version: 2.2.3
Release: 1%{?dist}%{?courier_release}
License: GPLv3
Group: System Environment/Libraries
URL: http://www.courier-mta.org/unicode/
Source: http://download.sourceforge.net/courier/courier-unicode-2.2.3.tar.bz2
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
BuildRequires: perl
BuildRequires: gcc-c++
BuildRequires: %{__make}

%if 0%{?compat:1}

%else
%package devel
Summary: Courier Unicode Library development files
Group: Development/Libraries
Requires: %{name} = 0:%{version}-%{release}
%endif

%description
This library implements several algorithms related to the Unicode
Standard.

This package installs only the run-time libraries needed by applications that
use this library. Install the "courier-unicode-devel" package if you want
to develop new applications using this library.

%if 0%{?compat:1}

%else
%description devel
This package contains development files for the Courier Unicode Library.
Install this package if you want to develop applications that uses this
unicode library.
%endif

%prep
%setup -q -n courier-unicode-2.2.3
%configure
%build
%{__make} -s %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
%{__make} install DESTDIR=$RPM_BUILD_ROOT

%if 0%{?compat:1}
find $RPM_BUILD_ROOT%{_libdir} -type l -print | xargs rm -f
rm -rf $RPM_BUILD_ROOT%{_includedir}
rm -f $RPM_BUILD_ROOT%{_libdir}/*.a
rm -f $RPM_BUILD_ROOT%{_libdir}/*.la
rm -rf $RPM_BUILD_ROOT%{_datadir}/aclocal
rm -rf $RPM_BUILD_ROOT%{_mandir}
%endif

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%if 0%{?compat:1}
%files
%defattr(-,root,root,-)
%{_libdir}/*.so.*

%else

%files
%defattr(-,root,root,-)
%doc README COPYING ChangeLog AUTHORS
%{_libdir}/*.so.*

%files devel
%{_mandir}/*/*
%{_includedir}/*
%{_libdir}/*.so
%{_libdir}/*.la
%{_libdir}/*.a
%{_datadir}/aclocal/*.m4
%endif

%changelog
* Sun Jan 12 2014 Sam Varshavchik <mrsam@octopus.email-scan.com> - 1.0
- Initial build.
