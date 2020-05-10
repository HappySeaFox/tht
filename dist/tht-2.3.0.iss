#define MyAppName "Trader's Home Task"
#define MyAppPublisher "Dmitry Baryshev"
#define MyAppURL "https://github.com/smoked-herring/tht"

[Setup]
AppId={{16AE5DDE-D073-4F5F-ABC3-11DD9FBF58E3}
AppName={#MyAppName}
AppVersion=2.4.0
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}/releases
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
LicenseFile=LICENSE.txt
OutputDir=.
OutputBaseFilename=tht-setup-2.3.0
SetupIconFile=chart.ico
Compression=lzma
SolidCompression=yes
UninstallDisplayName={#MyAppName}
UninstallDisplayIcon={app}\THT.exe
MinVersion=0,6.0

[Languages]
Name: "AAAenglish"; MessagesFile: "compiler:Default.isl,isl\English.isl"
Name: "BrazilianPortuguesexisl"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "Catalanxisl"; MessagesFile: "compiler:Languages\Catalan.isl"
Name: "Corsicanxisl"; MessagesFile: "compiler:Languages\Corsican.isl"
Name: "Czechxisl"; MessagesFile: "compiler:Languages\Czech.isl,isl\Czech.isl"
Name: "Danishxisl"; MessagesFile: "compiler:Languages\Danish.isl"
Name: "Dutchxisl"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "Finnishxisl"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "Frenchxisl"; MessagesFile: "compiler:Languages\French.isl"
Name: "Germanxisl"; MessagesFile: "compiler:Languages\German.isl"
Name: "Hebrewxisl"; MessagesFile: "compiler:Languages\Hebrew.isl,isl\Hebrew.isl"
Name: "Italianxisl"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "Japanesexisl"; MessagesFile: "compiler:Languages\Japanese.isl,isl\Japanese.isl"
Name: "Norwegianxisl"; MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "Polishxisl"; MessagesFile: "compiler:Languages\Polish.isl,isl\Polish.isl"
Name: "Portuguesexisl"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "Russianxisl"; MessagesFile: "compiler:Languages\Russian.isl,isl\Russian.isl"
Name: "Slovenianxisl"; MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "Spanishxisl"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "Turkishxisl"; MessagesFile: "compiler:Languages\Turkish.isl"
Name: "Ukrainianxisl"; MessagesFile: "compiler:Languages\Ukrainian.isl,isl\Ukrainian.isl"

[Types]
Name: "full"; Description: "{code:FullInstall}"
Name: "custom"; Description: "{code:CustomInstall}"; Flags: isCustom

[Components]
Name: plugins; Description: "{cm:Plugins}"; Types: full
Name: "plugins/addtickersfromfinviz"; Description: "Finviz (addtickersfrom-finviz)"; Flags: disablenouninstallwarning; Types: full custom
Name: "plugins/addtickersfrombriefingsplits"; Description: "Briefing Stock Splits (addtickersfrom-briefing-splits)"; Flags: disablenouninstallwarning; Types: full custom
Name: "plugins/stocksinplay"; Description: "Stocks In Play (addtickersfrom-stocksinplay)"; Flags: disablenouninstallwarning; Types: full custom
Name: "plugins/commonchat"; Description: "Jabber Chat (common-chat)"; Flags: disablenouninstallwarning; Types: full custom
Name: "plugins/commonfomc"; Description: "FOMC (common-fomc)"; Flags: disablenouninstallwarning; Types: full custom

[Files]
Source: "plugins/addtickersfrom-finviz.dll"; DestDir: "{app}/plugins"; Flags: ignoreversion; Components: plugins/addtickersfromfinviz
Source: "translations/addtickersfrom_finviz_*.qm"; DestDir: "{app}/translations"; Flags: ignoreversion; Components: plugins/addtickersfromfinviz
Source: "plugins/addtickersfrom-briefing-splits.dll"; DestDir: "{app}/plugins"; Flags: ignoreversion; Components: plugins/addtickersfrombriefingsplits
Source: "translations/addtickersfrom_briefing_splits_*.qm"; DestDir: "{app}/translations"; Flags: ignoreversion; Components: plugins/addtickersfrombriefingsplits
Source: "plugins/addtickersfrom-stocksinplay.dll"; DestDir: "{app}/plugins"; Flags: ignoreversion; Components: plugins/stocksinplay
Source: "translations/addtickersfrom_stocksinplay_*.qm"; DestDir: "{app}/translations"; Flags: ignoreversion; Components: plugins/stocksinplay
Source: "plugins/common-chat.dll"; DestDir: "{app}/plugins"; Flags: ignoreversion; Components: plugins/commonchat
Source: "translations/common_chat_*.qm"; DestDir: "{app}/translations"; Flags: ignoreversion; Components: plugins/commonchat
Source: "plugins/common-fomc.dll"; DestDir: "{app}/plugins"; Flags: ignoreversion; Components: plugins/commonfomc
Source: "translations/common_fomc_*.qm"; DestDir: "{app}/translations"; Flags: ignoreversion; Components: plugins/commonfomc

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; OnlyBelowVersion: 0,6.1

[Files]
Source: "THT.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "THT-lib.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "tickers.sqlite"; DestDir: "{app}"; Flags: ignoreversion
Source: "tickers.sqlite.timestamp"; DestDir: "{app}"; Flags: ignoreversion
Source: "styles/*"; DestDir: "{app}/styles"; Flags: ignoreversion recursesubdirs
Source: "LICENSE.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "LICENSE-LGPL-2.1.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "LICENSE-LGPL-3.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "translations/tht_*.qm"; DestDir: "{app}/translations"; Flags: ignoreversion
Source: "translations/qt_*.qm"; DestDir: "{app}/translations"; Flags: ignoreversion
Source: "QtCore4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "QtGui4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "QtNetwork4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "QtScript4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "QtSql4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "QtXml4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "QtXmlPatterns4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "libeay32.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "ssleay32.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "bearer\qgenericbearer4.dll"; DestDir: "{app}\bearer"; Flags: ignoreversion
Source: "bearer\qnativewifibearer4.dll"; DestDir: "{app}\bearer"; Flags: ignoreversion
Source: "imageformats\qico4.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "imageformats\qjpeg4.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "sqldrivers\qsqlite4.dll"; DestDir: "{app}\sqldrivers"; Flags: ignoreversion
Source: "libgcc_s_dw2-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\THT.exe"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\THT.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\THT.exe"; Tasks: quicklaunchicon

[Registry]
Root: HKCU; SubKey: "Software\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName: "THT preloader"; ValueData: "{code:AddQuotes|{app}\THT.exe} --preload"; Flags: uninsdeletevalue
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers"; ValueType: String; ValueName: "{app}\THT.exe"; ValueData: "RUNASADMIN"; Flags: uninsdeletekeyifempty uninsdeletevalue; MinVersion: 0,6.1

[Run]
;Filename: "{app}\THT.exe"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
Filename: "{#MyAppURL}/blob/master/CHANGELOG.md"; Flags: nowait shellexec

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
    Uninstall := RemoveQuotes(Uninstall)
    if (FileExists(Uninstall)) AND (not Exec(RemoveQuotes(Uninstall), '/VERYSILENT', '', SW_SHOWNORMAL, ewWaitUntilTerminated, ResultCode)) then begin
      MsgBox(SysErrorMessage(ResultCode), mbCriticalError, MB_OK);
      Abort();
    end;
  end;
end;
end;

function FullInstall(Param: String) : String;
begin
  result := SetupMessage(msgFullInstallation);
end;

function CustomInstall(Param: String) : String;
begin
  result := SetupMessage(msgCustomInstallation);
end;

function ReadyMemoComponents(Param: String) : String;
begin
  result := SetupMessage(msgReadyMemoComponents);
end;
