Name:    isis_nexus_streamer
Version: %{_version}
Release: %{_release}%{?dist}
%define dm_group_prefix /opt/dm_group/usr

Summary: ISIS NeXus Streamer
License: BSD 2-clause
URL:     https://github.com/ScreamingUdder/isis_nexus_streamer
Source:	 isis_nexus_streamer-%{version}.tar.gz

BuildArch: x86_64

%description
ISIS NeXus Streamer.


%package -n %{name}-data
Summary: ISIS NeXus Streamer sample data

BuildArch: noarch

%description -n %{name}-data
ISIS NeXus Streamer sample file.


%prep
%setup -q

%install
rm -rf %{buildroot}
install -d %{buildroot}%{dm_group_prefix}/bin
install -d %{buildroot}%{dm_group_prefix}/share/isis_nexus_streamer
cp bin/main_nexusPublisher %{buildroot}%{dm_group_prefix}/bin/nexusPublisher
cp bin/main_nexusSubscriber %{buildroot}%{dm_group_prefix}/bin/nexusSubscriber
cp share/SANS_test.nxs %{buildroot}%{dm_group_prefix}/share/isis_nexus_streamer/
cp share/LICENSE %{buildroot}%{dm_group_prefix}/share/isis_nexus_streamer/

%clean
rm -rf %{buildroot}

%files -n %{name}
%defattr(-,root,root)
%{dm_group_prefix}/bin/nexusPublisher
%{dm_group_prefix}/bin/nexusSubscriber
%doc %{dm_group_prefix}/share/isis_nexus_streamer/LICENSE

%files -n %{name}-data
%{dm_group_prefix}/share/isis_nexus_streamer/SANS_test.nxs


%changelog

* Fri Jan 13 2017 Afonso Mukai <afonso.mukai@esss.se> 0.1.0
- Change installation prefix to /opt/dm_group/usr

* Wed Dec 21 2016 Afonso Mukai <afonso.mukai@esss.se> 0.1.0
- Initial package
