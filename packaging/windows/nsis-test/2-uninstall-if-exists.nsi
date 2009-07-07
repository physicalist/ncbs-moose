!include "LogicLib.nsh"

Name "uninstall-if-exists"
OutFile "uninstall-if-exists.exe"

Section
	Push $0
	ReadRegStr $0 HKLM Software\MOOSE "Install_Dir"
	${If} $0 != ""
		MessageBox MB_YESNO|MB_ICONQUESTION "MOOSE is already installed at '$0'. Do you wish to uninstall it?" /SD IDYES IDNO nouninstall
			ExecWait '"$0\uninstall.exe"'
	${EndIf}
	nouninstall:
	Pop $0
SectionEnd
