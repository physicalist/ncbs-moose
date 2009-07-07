Name "My Application"
Caption 'installer caption'
OutFile 'test.exe'
LicenseData '${NSISDIR}\License.txt'
LicenseBkColor 0xFFFFFF
InstallButtonText "Next >"
ShowInstDetails show
XpStyle on
 
Page License
 
PageEx Custom
       Caption ': System Update Summary'
       PageCallbacks CustomCreate
PageExEnd
 
PageEx InstFiles
       Caption ': Mozilla Firefox Installation'
       CompletedText 'Mozilla Firefox Installation Completed'
       PageCallbacks pre1
PageExEnd
 
PageEx InstFiles
       Caption ': Mozilla Thunderbird Installation'
       CompletedText 'Mozilla Thunderbird Installation Completed'
       PageCallbacks pre2
PageExEnd
 
PageEx InstFiles
       Caption ': Prog 3 Installation'
       CompletedText 'Prog 3 Installation Completed'
       PageCallbacks pre3
PageExEnd
 
PageEx InstFiles
       Caption ': Prog 4 Installation'
       CompletedText 'Prog 4 Installation Completed'
       PageCallbacks pre4
PageExEnd
 
Section "Mozilla Firefox Installation" sec1
        # The section executed only when Firefox is not installed.
        # Add the proper code here to download + execute Firefox installer.
 
        DetailPrint "Please relax while downloading and installing Mozilla Firefox"
 
        sleep 6000 ;simulates the real code execution
SectionEnd
 
Section 'Mozilla Thunderbird Installation' sec2
        # The section executed only when Thunderbird is not installed.
        # Add the proper code here to download + execute Thunderbird installer.
 
        DetailPrint "Please relax while downloading and installing Mozilla Thunderbird"
 
        sleep 6000 ;simulates the real code execution
SectionEnd
 
Section 'Prog 3' sec3
        # The section executed only when Prog 3 is not installed.
        # Add the proper code here to download + execute Prog 3 installer.
 
        DetailPrint "Please relax while downloading and installing Prog 3"
 
        sleep 6000 ;simulates the real code execution
SectionEnd
 
Section 'Prog 4' sec4
        # The section executed only when Prog 4 is not installed.
        # Add the proper code here to download + execute Prog 4 installer.
 
        DetailPrint "Please relax while downloading and installing Prog 4"
 
        sleep 6000 ;simulates the real code execution
SectionEnd
 
function pre1
         # discover if Firefox is already installed
         ClearErrors
         readregstr $1 HKLM "SOFTWARE\Mozilla\Mozilla Firefox" "CurrentVersion"
         IfErrors done
         sectionsetflags ${sec1} 0
         SetAutoClose true
         Abort ;skip page
 
    done:
         SetAutoClose false
         sectionsetflags ${sec1} 1
         sectionsetflags ${sec2} 0
         sectionsetflags ${sec3} 0
         sectionsetflags ${sec4} 0
functionend
 
function pre2
         # discover if Thunderbird is already installed
         ClearErrors
         readregstr $1 HKLM "SOFTWARE\Mozilla\Mozilla Thunderbird" "CurrentVersion"
         IfErrors done
         sectionsetflags ${sec2} 0
         SetAutoClose true
         Abort ;skip page
 
    done:
         SetAutoClose false
         sectionsetflags ${sec1} 0
         sectionsetflags ${sec2} 1
         sectionsetflags ${sec3} 0
         sectionsetflags ${sec4} 0
functionend
 
function pre3
         # discover if Prog 3 is already installed
         ClearErrors
         readregstr $1 HKLM "SOFTWARE\Prog 3" "whatever"
         IfErrors done
         sectionsetflags ${sec3} 0
         SetAutoClose true
         Abort ;skip page
 
    done:
         SetAutoClose false
         sectionsetflags ${sec1} 0
         sectionsetflags ${sec2} 0
         sectionsetflags ${sec3} 1
         sectionsetflags ${sec4} 0
functionend
 
function pre4
         # discover if Prog 4 is already installed
         ClearErrors
         readregstr $1 HKLM "SOFTWARE\Prog 3" "whatever"
         IfErrors done
         sectionsetflags ${sec4} 0
         SetAutoClose true
         Abort ;skip page
 
    done:
         SetAutoClose false
         sectionsetflags ${sec1} 0
         sectionsetflags ${sec2} 0
         sectionsetflags ${sec3} 0
         sectionsetflags ${sec4} 1
functionend
 
function CustomCreate
         WriteIniStr '$PLUGINSDIR\custom.ini' 'Settings' 'NumFields' '2'
 
         WriteIniStr '$PLUGINSDIR\custom.ini' 'Field 1' 'Type' 'Label'
         WriteIniStr '$PLUGINSDIR\custom.ini' 'Field 1' 'Left' '5'
         WriteIniStr '$PLUGINSDIR\custom.ini' 'Field 1' 'Top' '5'
         WriteIniStr '$PLUGINSDIR\custom.ini' 'Field 1' 'Right' '-6'
         WriteIniStr '$PLUGINSDIR\custom.ini' 'Field 1' 'Bottom' '17'
         WriteIniStr '$PLUGINSDIR\custom.ini' 'Field 1' 'Text' \
         'Your system needs the following updates:'
 
         StrCpy $R0 ''
         ClearErrors
         readregstr $1 HKLM "SOFTWARE\Mozilla\Mozilla Firefox" "CurrentVersion"
         IfErrors 0 +2
         strcpy $R0 'Mozilla Firefox\r\n'
         ClearErrors
         readregstr $1 HKLM "SOFTWARE\Mozilla\Mozilla Thunderbird" "CurrentVersion"
         IfErrors 0 +2
         strcpy $R0 '$R0Mozilla Thunderbird\r\n'
         ClearErrors
         readregstr $1 HKLM "SOFTWARE\Prog 3" "whatever"
         IfErrors 0 +2
         strcpy $R0 '$R0Prog 3\r\n'
         ClearErrors
         readregstr $1 HKLM "SOFTWARE\Prog 4" "whatever"
         IfErrors 0 +2
         strcpy $R0 '$R0Prog 4\r\n'
 
         WriteIniStr '$PLUGINSDIR\custom.ini' 'Field 2' 'Type' 'Text'
         WriteIniStr '$PLUGINSDIR\custom.ini' 'Field 2' 'Left' '10'
         WriteIniStr '$PLUGINSDIR\custom.ini' 'Field 2' 'Top' '25'
         WriteIniStr '$PLUGINSDIR\custom.ini' 'Field 2' 'Right' '-10'
         WriteIniStr '$PLUGINSDIR\custom.ini' 'Field 2' 'Bottom' '105'
         WriteIniStr '$PLUGINSDIR\custom.ini' 'Field 2' 'State' '$R0'
         WriteIniStr '$PLUGINSDIR\custom.ini' 'Field 2' 'Flags' 'DISABLED|MULTILINE|NOWORDWRAP'
 
    push $0 ;park value to the stack
    InstallOptions::Dialog '$PLUGINSDIR\custom.ini'
    pop $0 ;get button action
    pop $0 ;get back value from stack
functionend
 
function .onInit
         initpluginsdir
         gettempfilename $0
         rename $0 '$PLUGINSDIR\custom.ini'
functionend