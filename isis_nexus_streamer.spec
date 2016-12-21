Name:    isis_nexus_streamer
Version: %{_version}
Release: %{_release}%{?dist}

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
install -d %{buildroot}/opt/dm_group/isis_nexus_streamer/bin
install -d %{buildroot}/opt/dm_group/isis_nexus_streamer/share
cp bin/main_nexusPublisher %{buildroot}/opt/dm_group/isis_nexus_streamer/bin/nexusPublisher
cp bin/main_nexusSubscriber %{buildroot}/opt/dm_group/isis_nexus_streamer/bin/nexusSubscriber
cp share/SANS_test.nxs %{buildroot}/opt/dm_group/isis_nexus_streamer/share/
cp share/LICENSE %{buildroot}/opt/dm_group/isis_nexus_streamer/share/

%clean
rm -rf %{buildroot}

%files -n %{name}
%defattr(-,root,root)
/opt/dm_group/isis_nexus_streamer/bin/nexusPublisher
/opt/dm_group/isis_nexus_streamer/bin/nexusSubscriber
%doc /opt/dm_group/isis_nexus_streamer/share/LICENSE

%files -n %{name}-data
/opt/dm_group/isis_nexus_streamer/share/SANS_test.nxs


%changelog

* Wed Dec 21 2016 Afonso Mukai <afonso.mukai@esss.se> - 1.8.18
- Initial package
