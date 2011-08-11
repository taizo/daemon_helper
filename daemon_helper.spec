%define version 0.0.1
%define release 1

Summary:	A tool monitoring daemon process
Name:		daemon_helper
Version:	%{version}
Release:	%{release}
Source0:	%{name}-%{version}.tar.gz
License:	BSD License
Packager:	Taizo ITO <taizoster@gmail.com>
Group:		Utilities/System
BuildRoot:	%{_tmppath}/%{name}-%{version}-%(id -u -n)

### include local configuration
%{?include_specopt}

%description
None.

%prep 
%setup -q

%build
%configure \
 --program-prefix=%{name}- \
 --program-transform-name=:

make %{?_smp_mflags} || make
## make check

%install
%makeinstall

mkdir -p $RPM_BUILD_ROOT/%{_sysconfdir}
mkdir -p $RPM_BUILD_ROOT/etc/init.d/
mkdir -p $RPM_BUILD_ROOT/etc/logrotate.d/
%{__install} -m600 src/helper/sample/helper.conf $RPM_BUILD_ROOT/%{_sysconfdir}
%{__sed} -e "s#@@PREFIX@@#%{_prefix}#" \
         -e "s#@@PROG_PREFIX@@#%{name}-#" \
  src/helper/sample/helper.init > $RPM_BUILD_ROOT/etc/init.d/%{name}
chmod 0550 $RPM_BUILD_ROOT/etc/init.d/%{name}
%{__sed} -e "s#@@PROG_PREFIX@@#%{name}-#" \
  src/helper/sample/helper.logrotate > $RPM_BUILD_ROOT/etc/logrotate.d/%{name}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(0700,root,root)
%{_sbindir}/*
%config(noreplace) %{_sysconfdir}/*
/etc/logrotate.d/*
/etc/init.d/*

%changelog
* Thu Aug 11 2011 Taizo ITO <taizoster@gmail.com>
- Initial 0.0.1 build.


%preun
if [ $1 = 0 ]; then
  /etc/init.d/%{name} stop >/dev/null 2>&1
  chkconfig --del %{name}
fi
