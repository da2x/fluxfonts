; Installer for Fluxfonts for Windows

;--------------------------------

!define PRODUCT "Fluxfonts"

Name "${PRODUCT} Installer"

OutFile "Release\fluxfonts_install.exe"

; Default installation directory
InstallDir $PROGRAMFILES\${PRODUCT}

; Store install dir in the registry
InstallDirRegKey HKLM "Software\${PRODUCT}" "Install_Dir"

; Request application privileges for UAC
RequestExecutionLevel admin


;--------------------------------

; Installer pages
Page directory
Page instfiles

; Uninstaller pages
UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------


Section "Installer"

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  ; Stop task if running (upgrades)
  ExecWait '$SYSDIR\schtasks.exe /End /tn ${PRODUCT}'
  ExecWait '$SYSDIR\schtasks.exe /Delete /tn ${PRODUCT} /f'

  ; Put file there
  File Release\fluxfonts.exe


  ; Install scheduled task
  ExecWait '$SYSDIR\schtasks.exe /Create /tn ${PRODUCT} /sc onstart /tr "$INSTDIR\fluxfonts.exe" /ru SYSTEM /rl HIGHEST'
  ExecWait '$SYSDIR\schtasks.exe /Run /tn ${PRODUCT} /i'

  ; Write the installation path to the registry
  WriteRegStr HKLM SOFTWARE\${PRODUCT} "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys to the registry
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "DisplayName" "${PRODUCT}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "UninstallString" '"$INSTDIR\fluxfonts_uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "NoRepair" 1
  WriteUninstaller "fluxfonts_uninstall.exe"

SectionEnd

;--------------------------------


Section "Uninstall"

  ReadEnvStr $0 "PROGRAMDATA"

  ExecWait '$SYSDIR\schtasks.exe /End /tn ${PRODUCT}'
  ExecWait '$SYSDIR\schtasks.exe /Delete /tn ${PRODUCT} /f'

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}"
  DeleteRegKey HKLM SOFTWARE\${PRODUCT}

  ; Remove files and uninstaller
  Delete $INSTDIR\fluxfonts.exe
  Delete $INSTDIR\fluxfonts_uninstall.exe

  IfFileExists $0 0
    Delete $0\fluxfonts\fonts
    Delete $0\fluxfonts\singleton.lock
    RMDir "$0\fluxfonts"

SectionEnd
