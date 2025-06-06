; minc.nsi

!include MUI.nsh
!include WinMessages.nsh
!include WinVer.nsh

Name "MinC"
OutFile test-6.1.exe
RequestExecutionLevel admin
Unicode True

; Use local drive as default, so the "Browse.." mechanism works
InstallDir "%SystemDrive%\MinC"

; Must be BMP3 and 200x57 pixels
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "images\puf800X689.bmp"
!define MUI_HEADERIMAGE_RIGHT

!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

#PEAddResource "minc-1.1.exe.manifest" "#24" "#2"

#!addplugindir "C:\Program Files\NSIS\Plugins"

Var SYSTEMDRIVE
Var USERNAME

;--------------------------------

Function .onInit
	ReadEnvStr $SYSTEMDRIVE SystemDrive
	ReadEnvStr $USERNAME USERNAME
	StrCpy $INSTDIR "$SYSTEMDRIVE\MinC"
FunctionEnd
Section

	# Set output path to the installation directory.
	SetOutPath $INSTDIR

	# Put files there
	File /r 'miniroot'

	ExecDos::exec /DETAILED 'cacls . /E /R "$USERNAME"'

	${If} ${AtMostWin2003}
		ExecDos::exec /DETAILED '.\miniroot\scacls . /E /G S-1-5-11:R'
	${EndIf}

	ExecDos::exec /DETAILED '.\miniroot\chmod -R 00755 miniroot'

SectionEnd
Section "Kernel" SecKernel

	# Component can not be disabled
	SectionIn RO

	File 'test61.tgz'
	File '*.cmd'

	# Make sure tar.exe does not see /dev/tty
	Delete .\dev\*.*

	DetailPrint "Installing kernel..."
	ExecDos::exec /DETAILED '.\install.cmd test61.tgz'

SectionEnd
Section
	DetailPrint "Configuring..."
	ExecDos::exec /DETAILED '.\postinstall.cmd osx'
	RMDir /r "$INSTDIR\miniroot"
	Delete "$INSTDIR\*.cmd"
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${SecKernel} "MinC kernel"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

