Name "read-registry"
OutFile "read-registry.exe"

Section
	ReadRegStr $0 HKLM Software\MOOSE "Install_Dir"
	MessageBox MB_OK "MOOSE is installed at: $0"
SectionEnd
