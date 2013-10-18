TARGET = THT
TEMPLATE = app

QT += core gui network xml

INCLUDEPATH += qtsingleapplication qxt colorpicker qtdropbox
DEPENDPATH += qtsingleapplication qxt colorpicker qtdropbox

SOURCES += main.cpp\
    tht.cpp \
    list.cpp \
    options.cpp \
    about.cpp \
    qtsingleapplication/qtsingleapplication.cpp \
    qtsingleapplication/qtlockedfile.cpp \
    qtsingleapplication/qtlockedfile_win.cpp \
    qtsingleapplication/qtlocalpeer.cpp \
    regionselect.cpp \
    savescreenshot.cpp \
    target.cpp \
    tickerinput.cpp \
    tickerinformationtooltip.cpp \
    updatechecker.cpp \
    listitem.cpp \
    tickerminiinput.cpp \
    tickerinformationfetcher.cpp \
    tickersdatabaseupdater.cpp \
    tickerneighbors.cpp \
    linkpointmanager.cpp \
    colorpicker/qtcolortriangle.cpp \
    colorpicker/colorviewer.cpp \
    colorpicker/colorpickerwidget.cpp \
    selectablelabel.cpp \
    screenshoteditorwidget.cpp \
    screenshoteditor.cpp \
    screenshotcommentinput.cpp \
    toolbuttonnomenuindicator.cpp \
    listdetails.cpp \
    inlinetextinput.cpp \
    tickercommentinput.cpp \
    pluginloader.cpp \
    pluginmanager.cpp \
    plugindetails.cpp \
    persistentselectiondelegate.cpp \
    masterdataevent.cpp \
    qtdropbox/qdropboxjson.cpp \
    qtdropbox/qdropboxfileinfo.cpp \
    qtdropbox/qdropboxfile.cpp \
    qtdropbox/qdropboxaccount.cpp \
    qtdropbox/qdropbox.cpp \
    dropboxuploader.cpp \
    excellinkingdetails.cpp \
    numericlabel.cpp

HEADERS += tht.h \
    list.h \
    options.h \
    about.h \
    qtsingleapplication/qtsingleapplication.h \
    qtsingleapplication/qtlockedfile.h \
    qtsingleapplication/qtlocalpeer.h \
    regionselect.h \
    savescreenshot.h \
    target.h \
    tickerinput.h \
    tickerinformationtooltip.h \
    updatechecker.h \
    listitem.h \
    tickerminiinput.h \
    tickerinformationfetcher.h \
    tickersdatabaseupdater.h \
    tickerneighbors.h \
    linkpointmanager.h \
    colorpicker/qtcolortriangle.h \
    colorpicker/colorviewer.h \
    colorpicker/colorpickerwidget.h \
    selectablelabel.h \
    screenshoteditorwidget.h \
    screenshoteditor.h \
    screenshotcommentinput.h \
    toolbuttonnomenuindicator.h \
    listdetails.h \
    inlinetextinput.h \
    ticker.h \
    tickercommentinput.h \
    pluginloader.h \
    pluginmanager.h \
    plugindetails.h \
    persistentselectiondelegate.h \
    masterdataevent.h \
    qtdropbox/qtdropbox_global.h \
    qtdropbox/qtdropbox.h \
    qtdropbox/qdropboxjson.h \
    qtdropbox/qdropboxfileinfo.h \
    qtdropbox/qdropboxfile.h \
    qtdropbox/qdropboxaccount.h \
    qtdropbox/qdropbox.h \
    dropboxuploader.h \
    excellinkingdetails.h \
    numericlabel.h

FORMS += tht.ui \
    list.ui \
    options.ui \
    about.ui \
    savescreenshot.ui \
    tickerinput.ui \
    tickerminiinput.ui \
    tickerneighbors.ui \
    screenshoteditor.ui \
    screenshotcommentinput.ui \
    inlinetextinput.ui \
    tickercommentinput.ui \
    pluginmanager.ui \
    plugindetails.ui \
    dropboxuploader.ui \
    excellinkingdetails.ui

RESOURCES += tht.qrc

LIBS += -lpsapi -lgdi32 -L$${OUT_PWD}/$(DESTDIR_TARGET)/.. -lTHT-lib

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += axcontainer
} else {
    CONFIG += qaxcontainer
}

OTHER_FILES += \
    tht.rc \
    README.txt \
    LICENSE.txt \
    THT-version.tag

RC_FILE = tht.rc

include(THT-version.pri)
include(THT-common.pri)

# network data
SVNROOT_FOR_COMMIT="svn+ssh://dmitrykx@svn.code.sf.net/p/tht/code"
SVNROOT_FOR_DOWNLOAD="http://sourceforge.net/p/tht/code/HEAD/tree/trunk"
HTTPROOT="http://sourceforge.net/p/tht"
DOWNLOADROOT="http://sourceforge.net/projects/tht"

# files to copy to the distribution
IMAGEPLUGINS=qico4.dll qjpeg4.dll
SQLPLUGINS=qsqlite4.dll
CODECPLUGINS=qcncodecs4.dll qjpcodecs4.dll qtwcodecs4.dll qkrcodecs4.dll
BEARERPLUGINS=qgenericbearer4.dll qnativewifibearer4.dll
QTLIBS=QtCore4.dll QtGui4.dll QtNetwork4.dll QtScript4.dll QtSql4.dll QtXml4.dll QtXmlPatterns4.dll
SSLLIBS=libeay32.dll ssleay32.dll
MINGW_BUILD_TYPE=sjlj
MINGWLIBS=libgcc_s_$${MINGW_BUILD_TYPE}-1.dll libwinpthread-1.dll libstdc++-6.dll
OTHERQMFILES=

for(l, LANGUAGES) {
    OTHERQMFILES += tht_lib_$${l}.qm
}

LICENSES=LICENSE.txt LICENSE-LGPL-2.1.txt LICENSE-LGPL-3.txt
USEUPX=y

COMPONENT1="Finviz"
COMPONENT1_TYPE="addtickersfromfinviz"
COMPONENT1_FILES=addtickersfrom-finviz.dll
for(l, LANGUAGES) {
    COMPONENT1_TRANSLATIONS += addtickersfrom_finviz_$${l}.qm
}

COMPONENT2="Briefing Stock Splits"
COMPONENT2_TYPE="addtickersfrombriefingsplits"
COMPONENT2_FILES=addtickersfrom-briefing-splits.dll
for(l, LANGUAGES) {
    COMPONENT2_TRANSLATIONS += addtickersfrom_briefing_splits_$${l}.qm
}

COMPONENT3="Stocks In Play"
COMPONENT3_TYPE="stocksinplay"
COMPONENT3_FILES=addtickersfrom-stocksinplay.dll
for(l, LANGUAGES) {
    COMPONENT3_TRANSLATIONS += addtickersfrom_stocksinplay_$${l}.qm
}

COMPONENT4="Jabber Chat"
COMPONENT4_TYPE="commonchat"
COMPONENT4_FILES=common-chat.dll
for(l, LANGUAGES) {
    COMPONENT4_TRANSLATIONS += common_chat_$${l}.qm
}

COMPONENT5="FOMC"
COMPONENT5_TYPE="commonfomc"
COMPONENT5_FILES=common-fomc.dll
for(l, LANGUAGES) {
    COMPONENT5_TRANSLATIONS += common_fomc_$${l}.qm
}

# list of components' suffixes to install
COMPONENTS=1 2 3 4 5

DEFINES += SVNROOT_FOR_DOWNLOAD=$$sprintf("\"\\\"%1\\\"\"", $$SVNROOT_FOR_DOWNLOAD)
DEFINES += HTTPROOT=$$sprintf("\"\\\"%1\\\"\"", $$HTTPROOT)
DEFINES += DOWNLOADROOT=$$sprintf("\"\\\"%1\\\"\"", $$DOWNLOADROOT)

tag.commands += $$mle(echo "$$VERSION"> "\"$${_PRO_FILE_PWD_}/THT-version.tag\"")
tag.commands += $$mle(svn -m "\"$$VERSION file tag\"" commit "\"$${_PRO_FILE_PWD_}/THT-version.tag\"")
tag.commands += $$mle(svn -m "\"$$VERSION tag\"" copy "\"$$SVNROOT_FOR_COMMIT/trunk\"" "\"$$SVNROOT_FOR_COMMIT/tags/$$VERSION\"")
QMAKE_EXTRA_TARGETS += tag

# copy database
QMAKE_POST_LINK += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\tickersdb\\tickers.sqlite\" \"$${OUT_PWD}/$(DESTDIR_TARGET)/..\")
QMAKE_POST_LINK += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\tickersdb\\tickers.sqlite.timestamp\" \"$${OUT_PWD}/$(DESTDIR_TARGET)/..\")

!isEmpty(ZIP) {
    message("7Z is found, will create custom dist targets")

    # source archive
    T="$${OUT_PWD}/tht-$$VERSION"
    distsrc.commands += $$mle(if exist \"$$T\" rd /S /Q \"$$T\")
    distsrc.commands += $$mle(svn export $$_PRO_FILE_PWD_ \"$$T\")
    distsrc.commands += $$mle(del /F /Q tht-$${VERSION}.zip)
    distsrc.commands += $$mle($$ZIP a -r -tzip -mx=9 tht-$${VERSION}.zip \"$$T\")
    distsrc.commands += $$mle(rd /S /Q \"$$T\")

    QMAKE_EXTRA_TARGETS += distsrc

    # standalone binary
    T="$${OUT_PWD}/tht-standalone-$$VERSION"

    distbin.commands += $$mle(if exist \"$$T\" rd /S /Q \"$$T\")
    distbin.commands += $$mle(mkdir \"$$T\")
    distbin.commands += $$mle(mkdir \"$$T/bearer\")
    distbin.commands += $$mle(mkdir \"$$T/codecs\")
    distbin.commands += $$mle(mkdir \"$$T/imageformats\")
    distbin.commands += $$mle(mkdir \"$$T/sqldrivers\")
    distbin.commands += $$mle(mkdir \"$$T/translations\")
    distbin.commands += $$mle(mkdir \"$$T/plugins\")

    # binary & necessary files
    distbin.commands += $$mle(copy /y \"$${OUT_PWD}/$(DESTDIR_TARGET)\" \"$$T\")
    distbin.commands += $$mle(copy /y \"$${OUT_PWD}/$(DESTDIR_TARGET)/../THT-lib.dll\" \"$$T\")

    for(ql, QTLIBS) {
        distbin.commands += $$mle(copy /y \"$$[QT_INSTALL_BINS]\\$$ql\" \"$$T\")
    }

    for(sl, SSLLIBS) {
        distbin.commands += $$mle(copy /y \"$$[QT_INSTALL_BINS]\\$$sl\" \"$$T\")
    }

    for(ml, MINGWLIBS) {
        distbin.commands += $$mle(copy /y \"$$GCCDIR\\$$ml\" \"$$T\")
    }

    for(bp, BEARERPLUGINS) {
        distbin.commands += $$mle(copy /y \"$$[QT_INSTALL_PLUGINS]\\bearer\\$$bp\" \"$$T/bearer\")
    }

    for(ip, IMAGEPLUGINS) {
        distbin.commands += $$mle(copy /y \"$$[QT_INSTALL_PLUGINS]\\imageformats\\$$ip\" \"$$T/imageformats\")
    }

    for(sp, SQLPLUGINS) {
        distbin.commands += $$mle(copy /y \"$$[QT_INSTALL_PLUGINS]\\sqldrivers\\$$sp\" \"$$T/sqldrivers\")
    }

    for(cp, CODECPLUGINS) {
        distbin.commands += $$mle(copy /y \"$$[QT_INSTALL_PLUGINS]\\codecs\\$$cp\" \"$$T/codecs\")
    }

    for(qm, QMFILES) {
        distbin.commands += $$mle(copy /y \"$$qm\" \"$$T/translations\")
    }

    for(l, LANGUAGES) {
        l=$$[QT_INSTALL_TRANSLATIONS]\\qt_$${l}.qm
        exists($$l) {
            distbin.commands += $$mle(copy /y \"$$l\" \"$$T/translations\")
        }
    }

    for(qm, OTHERQMFILES) {
        distbin.commands += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\ts\\$$qm\" \"$$T/translations\")
    }

    for(lc, LICENSES) {
        distbin.commands += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\$$lc\" \"$$T\")
    }

    for(c, COMPONENTS) {
        eval(COMPONENT_FILES=\$\${COMPONENT$${c}_FILES})
        eval(COMPONENT_TRANSLATIONS=\$\${COMPONENT$${c}_TRANSLATIONS})

        for(f, COMPONENT_FILES) {
            distbin.commands += $$mle(copy /y \"$${OUT_PWD}/$(DESTDIR_TARGET)/../$$f\" \"$$T/plugins\")
        }

        for(f, COMPONENT_TRANSLATIONS) {
            distbin.commands += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\ts\\$$f\" \"$$T/translations\")
        }
    }

    distbin.commands += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\tickersdb\\tickers.sqlite\" \"$$T\")
    distbin.commands += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\tickersdb\\tickers.sqlite.timestamp\" \"$$T\")

    # compress
    distbin.commands += $$mle(del /F /Q tht-standalone-$${VERSION}$${HOST64}.zip)
    distbin.commands += $$mle($$ZIP a -r -tzip -mx=9 tht-standalone-$${VERSION}$${HOST64}.zip \"$$T\")
    distbin.commands += $$mle(rd /S /Q \"$$T\")

    QMAKE_EXTRA_TARGETS += distbin
} else {
    warning("7Z is not found, will not create custom dist targets")
}

INNO_APPID="{{16AE5DDE-D073-4F5F-ABC3-11DD9FBF58E3}"

exists($$INNO) {
    message("Inno Setup is found, will create a setup file in a custom dist target")

    LANGS=$$system(dir /B \"$$INNO\\..\\Languages\")

    ISS="tht-$${VERSION}.iss"

    iss.commands += $$mle(echo $${LITERAL_HASH}define MyAppName \"Trader\'s Home Task\" > $$ISS)
    iss.commands += $$mle(echo $${LITERAL_HASH}define MyAppPublisher \"Dmitry Baryshev\" >> $$ISS)
    iss.commands += $$mle(echo $${LITERAL_HASH}define MyAppURL \"$$HTTPROOT\" >> $$ISS)

    iss.commands += $$mle(echo [Setup] >> $$ISS)

    !isEmpty(HOST64) {
        iss.commands += $$mle(echo ArchitecturesAllowed=x64 >> $$ISS)
        iss.commands += $$mle(echo ArchitecturesInstallIn64BitMode=x64 >> $$ISS)
    }

    iss.commands += $$mle(echo AppId=$$INNO_APPID >> $$ISS)
    iss.commands += $$mle(echo AppName={$${LITERAL_HASH}MyAppName} >> $$ISS)
    iss.commands += $$mle(echo AppVersion=$$VERSION >> $$ISS)
    iss.commands += $$mle(echo AppPublisher={$${LITERAL_HASH}MyAppPublisher} >> $$ISS)
    iss.commands += $$mle(echo AppPublisherURL={$${LITERAL_HASH}MyAppURL} >> $$ISS)
    iss.commands += $$mle(echo AppSupportURL={$${LITERAL_HASH}MyAppURL}/wiki/howto >> $$ISS)
    iss.commands += $$mle(echo AppUpdatesURL={$${LITERAL_HASH}MyAppURL} >> $$ISS)
    iss.commands += $$mle(echo DefaultDirName={pf}\\{$${LITERAL_HASH}MyAppName} >> $$ISS)
    iss.commands += $$mle(echo DefaultGroupName={$${LITERAL_HASH}MyAppName} >> $$ISS)
    iss.commands += $$mle(echo LicenseFile=$${_PRO_FILE_PWD_}\\LICENSE.txt >> $$ISS)
    iss.commands += $$mle(echo OutputDir=. >> $$ISS)
    iss.commands += $$mle(echo OutputBaseFilename=tht-setup-$${VERSION}$${HOST64} >> $$ISS)
    iss.commands += $$mle(echo SetupIconFile=$${_PRO_FILE_PWD_}\\images\\chart.ico >> $$ISS)
    iss.commands += $$mle(echo Compression=lzma >> $$ISS)
    iss.commands += $$mle(echo SolidCompression=yes >> $$ISS)
    iss.commands += $$mle(echo UninstallDisplayName={$${LITERAL_HASH}MyAppName} >> $$ISS)
    iss.commands += $$mle(echo UninstallDisplayIcon={app}\\$${TARGET}.exe >> $$ISS)
    iss.commands += $$mle(echo MinVersion="0,5.1" >> $$ISS)

    !isEmpty(SIGNTOOL):exists($$CERT) {
        iss.commands += $$mle(echo SignTool=bps sign /d \$\$qTrader\'s Home Task\$\$q /du \$\$q$$HTTPROOT\$\$q /f \$\$q$$CERT\$\$q /tr \$\$q$$RFC3161_SERVER\$\$q /v \$\$q\$\$f\$\$q >> $$ISS)
    }

    iss.commands += $$mle(echo [Languages] >> $$ISS)
    iss.commands += $$mle(echo Name: \"english\"; MessagesFile: \"compiler:Default.isl\" >> $$ISS)

    for(lng, LANGS) {
        lng = $$find(lng, .isl$)

        !isEmpty(lng) {
            lngname=$$lng
            lngname ~= s/\\./x
            iss.commands += $$mle(echo Name: \"$$lngname\"; MessagesFile: \"compiler:Languages\\$$lng\" >> $$ISS)
        }
    }

    # Types
    iss.commands += $$mle(echo [Types] >> $$ISS)

    iss.commands += $$mle(echo Name: \"full\"; Description: \"{code:FullInstall}\" >> $$ISS)
    iss.commands += $$mle(echo Name: \"custom\"; Description: \"{code:CustomInstall}\"; Flags: isCustom >> $$ISS)

    # Components
    iss.commands += $$mle(echo [Components] >> $$ISS)
    iss.commands += $$mle(echo Name: "plugins"; Description: \"{code:ReadyMemoComponents}\"; Types: full >> $$ISS)

    for(c, COMPONENTS) {
        eval(COMPONENT=\$\${COMPONENT$$c})
        eval(COMPONENT_TYPE=\$\${COMPONENT$${c}_TYPE})
        eval(COMPONENT_FILES=\$\${COMPONENT$${c}_FILES})

        SUFFIX=
        CF=

        for(cf, COMPONENT_FILES) {
            isEmpty(CF) {
                CF=$$replace(cf, \\.dll$, "")
            } else {
                SUFFIX="..."
                break()
            }
        }

        iss.commands += $$mle(echo Name: \"plugins/$$COMPONENT_TYPE\"; Description: \"$$COMPONENT ($$CF$$SUFFIX)\"; Flags: disablenouninstallwarning; Types: full custom >> $$ISS)
    }

    # Files of the components
    iss.commands += $$mle(echo [Files] >> $$ISS)

    for(c, COMPONENTS) {
        eval(COMPONENT_TYPE=\$\${COMPONENT$${c}_TYPE})
        eval(COMPONENT_FILES=\$\${COMPONENT$${c}_FILES})
        eval(COMPONENT_TRANSLATIONS=\$\${COMPONENT$${c}_TRANSLATIONS})

        for(f, COMPONENT_FILES) {
            iss.commands += $$mle(echo Source: \"$${OUT_PWD}/$(DESTDIR_TARGET)/../$$f\";  DestDir: \"{app}/plugins\"; Flags: ignoreversion; Components: plugins/$$COMPONENT_TYPE >> $$ISS)
        }

        for(f, COMPONENT_TRANSLATIONS) {
            iss.commands += $$mle(echo Source: \"$${_PRO_FILE_PWD_}\\ts\\$$f\";  DestDir: \"{app}/translations\"; Flags: ignoreversion; Components: plugins/$$COMPONENT_TYPE >> $$ISS)
        }
    }

    iss.commands += $$mle(echo [Tasks] >> $$ISS)
    iss.commands += $$mle(echo Name: \"desktopicon\"; Description: \"{cm:CreateDesktopIcon}\"; GroupDescription: \"{cm:AdditionalIcons}\" >> $$ISS)
    iss.commands += $$mle(echo Name: \"quicklaunchicon\"; Description: \"{cm:CreateQuickLaunchIcon}\"; GroupDescription: \"{cm:AdditionalIcons}\"; OnlyBelowVersion: "0,6.1" >> $$ISS)

    iss.commands += $$mle(echo [Files] >> $$ISS)
    iss.commands += $$mle(echo Source: \"$${OUT_PWD}/$(DESTDIR_TARGET)\"; DestDir: \"{app}\"; Flags: ignoreversion >> $$ISS)
    iss.commands += $$mle(echo Source: \"$${OUT_PWD}/$(DESTDIR_TARGET)/../THT-lib.dll\"; DestDir: \"{app}\"; Flags: ignoreversion >> $$ISS)
    iss.commands += $$mle(echo Source: \"$${_PRO_FILE_PWD_}\\tickersdb\\tickers.sqlite\"; DestDir: \"{app}\"; Flags: ignoreversion >> $$ISS)
    iss.commands += $$mle(echo Source: \"$${_PRO_FILE_PWD_}\\tickersdb\\tickers.sqlite.timestamp\"; DestDir: \"{app}\"; Flags: ignoreversion >> $$ISS)

    for(lc, LICENSES) {
        iss.commands += $$mle(echo Source: \"$${_PRO_FILE_PWD_}\\$$lc\"; DestDir: \"{app}\"; Flags: ignoreversion >> $$ISS)
    }

    for(qm, QMFILES) {
        iss.commands += $$mle(echo Source: \"$$qm\"; DestDir: \"{app}/translations\"; Flags: ignoreversion >> $$ISS)
    }

    for(l, LANGUAGES) {
        l=$$[QT_INSTALL_TRANSLATIONS]\\qt_$${l}.qm
        exists($$l) {
            iss.commands += $$mle(echo Source: \"$$l\"; DestDir: \"{app}/translations\"; Flags: ignoreversion >> $$ISS)
        }
    }

    for(qm, OTHERQMFILES) {
        iss.commands += $$mle(echo Source: \"$${_PRO_FILE_PWD_}\\ts\\$$qm\"; DestDir: \"{app}/translations\"; Flags: ignoreversion >> $$ISS)
    }

    for(ql, QTLIBS) {
        iss.commands += $$mle(echo Source: \"$$[QT_INSTALL_BINS]\\$$ql\"; DestDir: \"{app}\"; Flags: ignoreversion >> $$ISS)
    }

    for(sl, SSLLIBS) {
        iss.commands += $$mle(echo Source: \"$$[QT_INSTALL_BINS]\\$$sl\"; DestDir: \"{app}\"; Flags: ignoreversion >> $$ISS)
    }

    for(bp, BEARERPLUGINS) {
        iss.commands += $$mle(echo Source: \"$$[QT_INSTALL_PLUGINS]\\bearer\\$$bp\"; DestDir: \"{app}\\bearer\"; Flags: ignoreversion >> $$ISS)
    }

    for(ip, IMAGEPLUGINS) {
        iss.commands += $$mle(echo Source: \"$$[QT_INSTALL_PLUGINS]\\imageformats\\$$ip\"; DestDir: \"{app}\\imageformats\"; Flags: ignoreversion >> $$ISS)
    }

    for(sp, SQLPLUGINS) {
        iss.commands += $$mle(echo Source: \"$$[QT_INSTALL_PLUGINS]\\sqldrivers\\$$sp\"; DestDir: \"{app}\\sqldrivers\"; Flags: ignoreversion >> $$ISS)
    }

    for(cp, CODECPLUGINS) {
        iss.commands += $$mle(echo Source: \"$$[QT_INSTALL_PLUGINS]\\codecs\\$$cp\"; DestDir: \"{app}\\codecs\"; Flags: ignoreversion >> $$ISS)
    }

    for(ml, MINGWLIBS) {
        iss.commands += $$mle(echo Source: \"$$GCCDIR\\$$ml\"; DestDir: \"{app}\"; Flags: ignoreversion >> $$ISS)
    }

    iss.commands += $$mle(echo [Icons] >> $$ISS)
    iss.commands += $$mle(echo Name: \"{group}\\{$${LITERAL_HASH}MyAppName}\"; Filename: \"{app}\\$${TARGET}.exe\" >> $$ISS)
    iss.commands += $$mle(echo Name: \"{group}\\{cm:UninstallProgram","{$${LITERAL_HASH}MyAppName}}\"; Filename: \"{uninstallexe}\" >> $$ISS)
    iss.commands += $$mle(echo Name: \"{commondesktop}\\{$${LITERAL_HASH}MyAppName}\"; Filename: \"{app}\\$${TARGET}.exe\"; Tasks: desktopicon >> $$ISS)
    iss.commands += $$mle(echo Name: \"{userappdata}\\Microsoft\\Internet Explorer\\Quick Launch\\{$${LITERAL_HASH}MyAppName}\"; Filename: \"{app}\\$${TARGET}.exe\"; Tasks: quicklaunchicon >> $$ISS)

    iss.commands += $$mle(echo [Registry] >> $$ISS)
    iss.commands += $$mle(echo Root: HKCU32; SubKey: Software\\Microsoft\\Windows\\CurrentVersion\\Run; ValueType: string; ValueName: \"$${TARGET} preloader\"; ValueData: \"{code:AddQuotes|{app}\\$${TARGET}.exe} --preload\"; Flags: uninsdeletevalue >> $$ISS)

    iss.commands += $$mle(echo [Run] >> $$ISS)
    iss.commands += $$mle(echo ;Filename: \"{app}\\$${TARGET}.exe\"; Description: \"{cm:LaunchProgram","{$${LITERAL_HASH}StringChange(MyAppName"," \'&\'"," \'&&\')}}\"; Flags: nowait postinstall skipifsilent >> $$ISS)
    iss.commands += $$mle(echo Filename: \"{$${LITERAL_HASH}MyAppURL}/wiki/howto\"; Flags: nowait shellexec >> $$ISS)
    iss.commands += $$mle(echo Filename: \"{$${LITERAL_HASH}MyAppURL}/wiki/changelog\"; Flags: nowait shellexec >> $$ISS)

    iss.commands += $$mle(echo [Code] >> $$ISS)
    iss.commands += $$mle(echo procedure CurStepChanged(CurStep: TSetupStep); >> $$ISS)
    iss.commands += $$mle(echo var >> $$ISS)
    iss.commands += $$mle(echo ResultCode: Integer; >> $$ISS)
    iss.commands += $$mle(echo Uninstall: String; >> $$ISS)
    iss.commands += $$mle(echo UninstallQuery : String; >> $$ISS)
    iss.commands += $$mle(echo begin >> $$ISS)
    iss.commands += $$mle(echo   UninstallQuery := ExpandConstant(\'Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{$${LITERAL_HASH}emit SetupSetting(\"AppId\")}_is1\'); >> $$ISS)

    iss.commands += $$mle(echo   if (CurStep = ssInstall) then begin >> $$ISS)
    iss.commands += $$mle(echo     if RegQueryStringValue(HKLM, UninstallQuery, \'UninstallString\', Uninstall) >> $$ISS)
    iss.commands += $$mle(echo        or RegQueryStringValue(HKCU, UninstallQuery, \'UninstallString\', Uninstall) then begin >> $$ISS)
    iss.commands += $$mle(echo       if not Exec(RemoveQuotes(Uninstall), \'/VERYSILENT\', \'\', SW_SHOWNORMAL, ewWaitUntilTerminated, ResultCode) then begin >> $$ISS)
    iss.commands += $$mle(echo         MsgBox(SysErrorMessage(ResultCode), mbCriticalError, MB_OK); >> $$ISS)
    iss.commands += $$mle(echo         Abort(); >> $$ISS)
    iss.commands += $$mle(echo       end; >> $$ISS)
    iss.commands += $$mle(echo     end; >> $$ISS)
    iss.commands += $$mle(echo   end; >> $$ISS)
    iss.commands += $$mle(echo end; >> $$ISS)

    iss.commands += $$mle(echo function FullInstall(Param: String) : String; >> $$ISS)
    iss.commands += $$mle(echo begin >> $$ISS)
    iss.commands += $$mle(echo result := SetupMessage(msgFullInstallation); >> $$ISS)
    iss.commands += $$mle(echo end; >> $$ISS)

    iss.commands += $$mle(echo function CustomInstall(Param: String) : String; >> $$ISS)
    iss.commands += $$mle(echo begin >> $$ISS)
    iss.commands += $$mle(echo result := SetupMessage(msgCustomInstallation); >> $$ISS)
    iss.commands += $$mle(echo end; >> $$ISS)

    iss.commands += $$mle(echo function ReadyMemoComponents(Param: String) : String; >> $$ISS)
    iss.commands += $$mle(echo begin >> $$ISS)
    iss.commands += $$mle(echo result := SetupMessage(msgReadyMemoComponents); >> $$ISS)
    iss.commands += $$mle(echo end; >> $$ISS)

    QMAKE_EXTRA_TARGETS += iss
    QMAKE_EXTRA_TARGETS *= distbin

    !isEmpty(SIGNTOOL):exists($$CERT) {
        ADD="\"/sbps=$$SIGNTOOL \$\$p\""
    }

    distbin.depends += iss
    distbin.commands += $$mle(\"$$INNO\" /o. $$ADD \"$$ISS\")
    distbin.commands += $$mle(del /F /Q \"$$ISS\")
} else {
    warning("Inno Setup is not found, will not create a setup file in a custom dist target")
}
