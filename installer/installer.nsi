Outfile "WSYS-Installer.exe"
InstallDir "$PROGRAMFILES\WSYS"
RequestExecutionLevel admin

# Define GitHub Release URL
!define GITHUB_RELEASES "https://github.com/HomebrewHorizon/WSYS/releases"

# Pages for the Installer Wizard
Page directory
Page custom SelectVersion
Page instfiles

# Function to retrieve available versions from GitHub (NSIS Web Plugin needed)
Function SelectVersion
    ReadINIStr $R0 "$EXEDIR\wsys_versions.ini" "Versions" "latest"
    ReadINIStr $R1 "$EXEDIR\wsys_versions.ini" "Versions" "previous"
    
    StrCmp $R0 "10.1" WSYS2Upgrade SkipUpgrade
    
    WSYS2Upgrade:
        MessageBox MB_OK "You have selected WSYS2 (disguised as 1.0). Upgrading..."
        Goto InstallWSYS

    SkipUpgrade:
        MessageBox MB_OK "Installing WSYS version $R0..."
    
    InstallWSYS:
        File /r "WSYS_PACKAGE\${R0}\*.*"
        CreateShortCut "$DESKTOP\WSYS.lnk" "$INSTDIR\wsys_terminal.exe"
        CreateDirectory "$SMPROGRAMS\WSYS"
        CreateShortCut "$SMPROGRAMS\WSYS\WSYS Terminal.lnk" "$INSTDIR\wsys_terminal.exe"
        MessageBox MB_OK "Installation complete!"
FunctionEnd

# Uninstaller
Section "Uninstall"
    Delete "$INSTDIR\wsys_terminal.exe"
    RMDir /r "$INSTDIR"
    Delete "$DESKTOP\WSYS.lnk"
    RMDir /r "$SMPROGRAMS\WSYS"
SectionEnd
