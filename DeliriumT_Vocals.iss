#define MyAppName "DeliriumT Vocals"
#define MyAppVersion "1.0.0"
#define MyPublisher "Dark Grave"
#define PluginBundle "..\build\DeliriumTVocals_artefacts\Release\VST3\DeliriumT Vocals.vst3"

[Setup]
AppId={{A8A6C24D-5F13-4B3B-9B11-0D3111A0CA15}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyPublisher}
DefaultDirName={commoncf}\VST3
DisableProgramGroupPage=yes
ArchitecturesInstallIn64BitMode=x64
OutputDir=Output
OutputBaseFilename=DeliriumT_Vocals_Windows_x64_Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern

[Files]
Source: "{#PluginBundle}\*"; DestDir: "{app}\DeliriumT Vocals.vst3"; Flags: recursesubdirs createallsubdirs ignoreversion

[Code]
function InitializeSetup(): Boolean;
begin
  Result := IsWin64;
  if not Result then
    MsgBox('DeliriumT Vocals requires 64-bit Windows 10 or 11.', mbError, MB_OK);
end;
