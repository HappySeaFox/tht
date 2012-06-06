;
; THT setup file
;

#define MyAppName "Trader's Home Task"
#define MyAppPublisher "Dmitry Baryshev"
#define MyAppURL "https://code.google.com/p/traders-home-task-ng"
#define MyAppExeName "THT.exe"

; NOTE: version must be defined outside with /d
; #define MyAppVersion "0.8.4"

#define MyAppSrc "F:\trading\soft\THT"
#define MyAppDist "F:\trading\soft\THT-dist"
#define MyAppBuildDir "F:\trading\soft\THT-build-desktop"
#define QtDir "F:\Qt\Desktop\Qt\4.8.1\mingw"
#define MingwDir "C:\MinGW"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{16AE5DDE-D073-4F5F-ABC3-11DD9FBF58E3}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
LicenseFile={#MyAppDist}\license\gpl-3.0-standalone.rtf
OutputDir={#MyAppDist}
OutputBaseFilename=tht-setup-{#MyAppVersion}
SetupIconFile={#MyAppSrc}\images\chart.ico
Compression=lzma
SolidCompression=yes
UninstallDisplayName={#MyAppName}
UninstallDisplayIcon={app}\{#MyAppExeName}
MinVersion=0,5.1

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "basque"; MessagesFile: "compiler:Languages\Basque.isl"
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "catalan"; MessagesFile: "compiler:Languages\Catalan.isl"
Name: "czech"; MessagesFile: "compiler:Languages\Czech.isl"
Name: "danish"; MessagesFile: "compiler:Languages\Danish.isl"
Name: "dutch"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "finnish"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"
Name: "hebrew"; MessagesFile: "compiler:Languages\Hebrew.isl"
Name: "hungarian"; MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "italian"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "japanese"; MessagesFile: "compiler:Languages\Japanese.isl"
Name: "norwegian"; MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "polish"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "portuguese"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "serbiancyrillic"; MessagesFile: "compiler:Languages\SerbianCyrillic.isl"
Name: "serbianlatin"; MessagesFile: "compiler:Languages\SerbianLatin.isl"
Name: "slovak"; MessagesFile: "compiler:Languages\Slovak.isl"
Name: "slovenian"; MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "ukrainian"; MessagesFile: "compiler:Languages\Ukrainian.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "{#MyAppBuildDir}\release\THT.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyAppSrc}\tickersdb\tickers.sqlite"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyAppSrc}\tickersdb\tickers.sqlite.timestamp"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyAppSrc}\LICENSE.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyAppSrc}\LICENSE-LGPL.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyAppSrc}\ts\ru.qm"; DestDir: "{app}/translations"; Flags: ignoreversion
Source: "{#MyAppSrc}\ts\uk.qm"; DestDir: "{app}/translations"; Flags: ignoreversion
Source: "{#MyAppSrc}\ts\qt_ru.qm"; DestDir: "{app}/translations"; Flags: ignoreversion
Source: "{#MyAppSrc}\ts\qt_uk.qm"; DestDir: "{app}/translations"; Flags: ignoreversion
Source: "{#QtDir}\bin\QtCore4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\bin\QtGui4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\bin\QtNetwork4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\bin\QtSql4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\plugins\imageformats\qgif4.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtDir}\plugins\imageformats\qico4.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtDir}\plugins\imageformats\qjpeg4.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtDir}\plugins\imageformats\qtga4.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtDir}\plugins\imageformats\qtiff4.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtDir}\plugins\sqldrivers\qsqlite4.dll"; DestDir: "{app}\sqldrivers"; Flags: ignoreversion
Source: "{#MingwDir}\bin\libgcc_s_dw2-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MingwDir}\bin\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MingwDir}\bin\mingwm10.dll"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Registry]
Root: HKCU32; SubKey: Software\Microsoft\Windows\CurrentVersion\Run; ValueType: string; ValueName: "THT preloader"; ValueData: "{code:AddQuotes|{app}\{#MyAppExeName}} --preload"; Flags: uninsdeletevalue

[Run]
;Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
Filename: "{#MyAppURL}/wiki/howto"; Flags: nowait shellexec
