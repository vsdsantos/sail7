# This is a spec file for the installation of sail7 v0
%define name sail7
%define release 0
%define version 0.02

# norootforbuild

BuildRoot: %{_tmppath}/%{name}-%{version}
Summary: sail7 is an analysis tool for sails
License: limited_use
Name: %{name}
Version: %{version}
Release: %{release}
Source: %{name}-%{version}.tar.gz
Packager: windsoarer
Group: Applications/Engineering
%if 0%{?fedora_version}  
BuildRequires: qt-devel >= 4.7.0 gcc-c++  , Mesa-devel
%else  
BuildRequires: libqt4-devel >= 4.7.0 gcc-c++  , Mesa-devel
%endif  

%description
sail7 is a tool designed for the analysis of sail boat performance.
It is based on the VLM and panel methods developed for the program XFLR5. 
The current version is released in alpha phase for testing and feedback.
Further development will depend on user interest.
For information, this program has been initially developed as version 7 of XFLR5, hence the name. 


%prep


%setup -q

%build 
%if 0%{?fedora_version}  
qmake-qt4 -makefile %{name}-%{version}.pro 'target.path = %{buildroot}%{_bindir}'  
qmake-qt4 'target.path = %{buildroot}%{_bindir}'  
%else  
qmake -makefile %{name}-%{version}.pro 'target.path = %{buildroot}%{_bindir}'  
qmake 'target.path = %{buildroot}%{_bindir}'   
%endif  

echo $RPM_BUILD_ROOT 
echo %{buildroot}%{_bindir} 

make 

 
%install
%__mkdir -p %{buildroot}%{_bindir} 
%__mkdir -p %{buildroot}/usr/share/%{name} 
%__mkdir -p %{buildroot}/usr/share/applications
%__mkdir -p %{buildroot}/usr/share/pixmaps 
%__mkdir -p %{buildroot}%{_docdir}/%{name}  
make install DESTDIR=$RPM_BUILD_ROOT $INSTALL_TARGET 
#make install  DESTDIR=%{buildroot}%{_bindir} $INSTALL_TARGET
%__install -m 644 images/%{name}.png    %{buildroot}%{_datadir}/pixmaps/%{name}.png  
%__install -m 644 suse/%{name}.desktop  %{buildroot}%{_datadir}/applications  



%files
%defattr(-,root,root)
  
#%if 0%{suse_version} > 1110 || 0%{fedora_version} || 0%{mandriva_version}  
%{_datadir}/applications/%{name}.desktop  
#%endif  
  
%{_datadir}/pixmaps/%{name}.png  
%{_datadir}/%{name}  

%doc license.txt
%{_bindir}/%{name} 

%clean
#%{__rm} -rf $RPM_BUILD_ROOT 

echo %{_bindir}/%{name} 


%changelog
*Sat Jun 2 2012 A. Deperrois
sail7 0.02
-first operational release for alpha-testing

*Wed Jan 4 2012 A. Deperrois
sail7 0.01
- concept demonstration


