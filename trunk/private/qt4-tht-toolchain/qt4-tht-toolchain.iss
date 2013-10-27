#define MyAppName "Qt4 THT toolchain"
#define MyAppInstallDir "Qt4-THT-toolchain"
#define MyAppPublisher "Dmitry Baryshev"
#define MyAppURL "http://sourceforge.net/projects/tht"
#define MyAppVersion "4.8.5"

[Setup]
AppId={{6742AEC5-AAA7-45F6-B430-CB004151C983}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}/files/THT-development
DefaultDirName={sd}\{#MyAppInstallDir}
DefaultGroupName={#MyAppName}
LicenseFile=qt4-tht-toolchain-LICENSE.txt
OutputDir=.
OutputBaseFilename=qt4-tht-toolchain-{#MyAppVersion}
SetupIconFile=qt4-tht-toolchain.ico
Compression=lzma
SolidCompression=yes
UninstallDisplayName={#MyAppName}
UninstallDisplayIcon={app}\qt4-tht-toolchain.ico
MinVersion=0,5.1
ChangesEnvironment=yes
; uncomment this on x86_64
;ArchitecturesAllowed=x64
;ArchitecturesInstallIn64BitMode=x64

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "BrazilianPortuguesexisl"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "Catalanxisl"; MessagesFile: "compiler:Languages\Catalan.isl"
Name: "Corsicanxisl"; MessagesFile: "compiler:Languages\Corsican.isl"
Name: "Czechxisl"; MessagesFile: "compiler:Languages\Czech.isl"
Name: "Danishxisl"; MessagesFile: "compiler:Languages\Danish.isl"
Name: "Dutchxisl"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "Finnishxisl"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "Frenchxisl"; MessagesFile: "compiler:Languages\French.isl"
Name: "Germanxisl"; MessagesFile: "compiler:Languages\German.isl"
Name: "Greekxisl"; MessagesFile: "compiler:Languages\Greek.isl"
Name: "Hebrewxisl"; MessagesFile: "compiler:Languages\Hebrew.isl"
Name: "Hungarianxisl"; MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "Italianxisl"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "Japanesexisl"; MessagesFile: "compiler:Languages\Japanese.isl"
Name: "Norwegianxisl"; MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "Polishxisl"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "Portuguesexisl"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "Russianxisl"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "SerbianCyrillicxisl"; MessagesFile: "compiler:Languages\SerbianCyrillic.isl"
Name: "SerbianLatinxisl"; MessagesFile: "compiler:Languages\SerbianLatin.isl"
Name: "Slovenianxisl"; MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "Spanishxisl"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "Ukrainianxisl"; MessagesFile: "compiler:Languages\Ukrainian.isl"

[Tasks]

[Files]
Source: "qt4-tht-toolchain\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs overwritereadonly uninsremovereadonly
Source: "qt4-tht-toolchain.ico"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{app}\bin;{olddata}"; Check: NeedsAddPath(ExpandConstant('{app}\bin'))
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{app}\extra;{olddata}"; Check: NeedsAddPath(ExpandConstant('{app}\extra'))
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{app}\extra\mingw\bin;{olddata}"; Check: NeedsAddPath(ExpandConstant('{app}\extra\mingw\bin'))

[Run]

[Code]
procedure CurStepChanged(CurStep: TSetupStep);
var
ResultCode: Integer;
Uninstall: String;
UninstallQuery : String;

begin
  UninstallQuery := ExpandConstant('Software\Microsoft\Windows\CurrentVersion\Uninstall\{#emit SetupSetting("AppId")}_is1');
  if (CurStep = ssInstall) then begin
    if RegQueryStringValue(HKLM, UninstallQuery, 'UninstallString', Uninstall)
      or RegQueryStringValue(HKCU, UninstallQuery, 'UninstallString', Uninstall) then begin
      if not Exec(RemoveQuotes(Uninstall), '/VERYSILENT', '', SW_SHOWNORMAL, ewWaitUntilTerminated, ResultCode) then begin
        MsgBox(SysErrorMessage(ResultCode), mbCriticalError, MB_OK);
        Abort();
      end;
    end;
  end
  else if (CurStep = ssPostInstall) then begin
  // post-install
  end;
end;

// http://stackoverflow.com/questions/3304463/how-do-i-modify-the-path-environment-variable-when-running-an-inno-setup-install
function NeedsAddPath(Param: string): boolean;
var
  OrigPath: string;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', OrigPath)
  then begin
    Result := True;
    exit;
  end;
  Result := Pos(';' + UpperCase(Param) + ';', ';' + UpperCase(OrigPath) + ';') = 0;  
  if Result = True then
     Result := Pos(';' + UpperCase(Param) + '\;', ';' + UpperCase(OrigPath) + ';') = 0; 
end;
