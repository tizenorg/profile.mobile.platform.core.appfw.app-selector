Name:       org.tizen.app-selector
Summary:    Application selector
Version:    0.1.61
Release:    1
Group:      Application Framework/Notifications
License:    Flora
Source0:    %{name}-%{version}.tar.gz

%if "%{?profile}" == "wearable"
ExcludeArch: %{arm} %ix86 x86_64
%endif

BuildRequires:  cmake
BuildRequires:  gettext-tools
BuildRequires:  edje-tools
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(aul)
BuildRequires:  pkgconfig(rua)
BuildRequires:  pkgconfig(eina)
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(ecore-file)
BuildRequires:  pkgconfig(edje)
BuildRequires:  pkgconfig(ail)
BuildRequires:  pkgconfig(appsvc)
BuildRequires:  pkgconfig(efl-extension)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  hash-signer


%description
SLP application selector
 
 
%prep
%setup -q


%build
%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
%endif


export CFLAGS="$CFLAGS -Wall -Werror -Wno-unused-function"
cmake -DCMAKE_INSTALL_PREFIX=%{_prefix} \
	-DAPP_SELECTOR_VERSION="%{version}" \
	.

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install
#Signing
%define tizen_sign_base %{_prefix}/apps/%{name}
%define tizen_sign 1
%define tizen_author_sign 1
%define tizen_dist_sign 1
%define tizen_sign_level platform
mkdir -p %{buildroot}%{_datadir}/license
install LICENSE %{buildroot}%{_datadir}/license/%{name}

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_prefix}/apps/%{name}/bin/*
%{_prefix}/apps/%{name}/res/edje/*
%{_prefix}/apps/%{name}/res/locale/*/*/*.mo
%{_datadir}/packages/%{name}.xml
#Signing
%{_prefix}/apps/%{name}/author-signature.xml
%{_prefix}/apps/%{name}/signature1.xml
%{_datadir}/license/%{name}
