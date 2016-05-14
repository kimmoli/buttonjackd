#
# buttonjackd spec
# (C) kimmoli 2016
#

Name:       buttonjackd

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}

Summary:    Button Jack daemon for headset buttons
Version:    0.0.devel
Release:    1
Group:      Qt/Qt
License:    LICENSE
URL:        https://github.com/kimmoli/buttonjackd
Source0:    %{name}-%{version}.tar.bz2

BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)

%description
%{summary}

%prep
%setup -q -n %{name}-%{version}

%build

%qtc_qmake5 SPECVERSION=%{version}

%qtc_make %{?_smp_mflags}

%install
rm -rf %{buildroot}

%qmake5_install

%preun
# in case of complete removal, stop and disable
if [ "$1" = "0" ]; then
  systemctl-user stop buttonjackd
  systemctl-user disable buttonjackd
fi

%post
systemctl-user daemon-reload
systemctl-user start buttonjackd
systemctl-user enable buttonjackd

%pre
# In case of update, stop first
if [ "$1" = "2" ]; then
  systemctl-user stop buttonjackd
  systemctl-user disable buttonjackd
fi

%files
%defattr(644,root,root,755)
%attr(755,root,root) %{_bindir}/%{name}
/etc/systemd/user/

