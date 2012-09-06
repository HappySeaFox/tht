#-------------------------------------------------
#
# Project created by QtCreator 2012-01-29T00:47:02
#
#-------------------------------------------------

TARGET = THT
TEMPLATE = app

!win32 {
    error("$$TARGET requires Windows platform")
}

QT += core gui network sql
CONFIG += warn_on
QMAKE_CXXFLAGS_WARN_ON *= -Wextra -Wno-missing-field-initializers

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# THT version
NVER1=1
NVER2=2
NVER3=0

VERSION=$$sprintf("%1.%2.%3", $$NVER1, $$NVER2, $$NVER3)

DEFINES += NVER1=$$NVER1
DEFINES += NVER2=$$NVER2
DEFINES += NVER3=$$NVER3
DEFINES += NVER_STRING=$$sprintf("\"\\\"%1\\\"\"", $$VERSION)

DEFINES += TARGET=$$TARGET

INCLUDEPATH += . qtsingleapplication qxt colorpicker
DEPENDPATH += . qtsingleapplication qxt colorpicker

# require at least Windows XP
DEFINES += _WIN32_WINNT=0x0501 WINVER=0x0501

SOURCES += main.cpp\
    tht.cpp \
    list.cpp \
    options.cpp \
    about.cpp \
    settings.cpp \
    qtsingleapplication/qtsingleapplication.cpp \
    qtsingleapplication/qtlockedfile.cpp \
    qtsingleapplication/qtlockedfile_win.cpp \
    qtsingleapplication/qtlocalpeer.cpp \
    regionselect.cpp \
    savescreenshot.cpp \
    qxt/qxtglobalshortcut_win.cpp \
    qxt/qxtglobalshortcut.cpp \
    qxt/qxtglobal.cpp \
    target.cpp \
    tickerinput.cpp \
    tickerinformationtooltip.cpp \
    updatechecker.cpp \
    listitem.cpp \
    searchticker.cpp \
    uppercasevalidator.cpp \
    tickerminiinput.cpp \
    networkaccess.cpp \
    tickerinformationfetcher.cpp \
    tickersdatabaseupdater.cpp \
    tickerneighbors.cpp \
    sqltools.cpp \
    linkpointmanager.cpp \
    tools.cpp \
    finvizurlmanager.cpp \
    datamanagerbase.cpp \
    finvizdownloader.cpp \
    csvreader.cpp \
    finvizlinkselector.cpp \
    colorpicker/qtcolortriangle.cpp \
    colorpicker/colorviewer.cpp \
    colorpicker/colorpickerwidget.cpp \
    selectablelabel.cpp \
    screenshoteditorwidget.cpp \
    screenshoteditor.cpp \
    screenshotcommentinput.cpp \
    toolbuttonnomenuindicator.cpp

HEADERS += tht.h \
    list.h \
    options.h \
    about.h \
    settings.h \
    qtsingleapplication/qtsingleapplication.h \
    qtsingleapplication/QtSingleApplication \
    qtsingleapplication/qtlockedfile.h \
    qtsingleapplication/QtLockedFile \
    qtsingleapplication/qtlocalpeer.h \
    regionselect.h \
    savescreenshot.h \
    qxt/qxtglobalshortcut_p.h \
    qxt/qxtglobalshortcut.h \
    qxt/qxtglobal.h \
    qxt/QxtGlobalShortcut \
    qxt/QxtGlobalShortcut \
    target.h \
    tickerinput.h \
    tickerinformationtooltip.h \
    updatechecker.h \
    listitem.h \
    searchticker.h \
    uppercasevalidator.h \
    tickerminiinput.h \
    networkaccess.h \
    tickerinformationfetcher.h \
    tickersdatabaseupdater.h \
    tickerneighbors.h \
    sqltools.h \
    linkpointmanager.h \
    tools.h \
    finvizurlmanager.h \
    datamanagerbase.h \
    finvizdownloader.h \
    csvreader.h \
    finvizlinkselector.h \
    colorpicker/qtcolortriangle.h \
    colorpicker/colorviewer.h \
    colorpicker/colorpickerwidget.h \
    selectablelabel.h \
    screenshoteditorwidget.h \
    screenshoteditor.h \
    screenshotcommentinput.h \
    toolbuttonnomenuindicator.h

FORMS += tht.ui \
    list.ui \
    options.ui \
    about.ui \
    savescreenshot.ui \
    tickerinput.ui \
    searchticker.ui \
    tickerminiinput.ui \
    tickerneighbors.ui \
    datamanagerbase.ui \
    finvizdownloader.ui \
    finvizlinkselector.ui \
    screenshoteditor.ui \
    screenshotcommentinput.ui

RESOURCES += tht.qrc

LIBS += -lpsapi

RC_FILE = tht.rc

OTHER_FILES += \
    tht.rc \
    README.txt \
    LICENSE.txt \
    THT-version.tag

TRANSLATIONS += ts/tht_ru.ts ts/tht_uk.ts ts/tht_en_US.ts

# search an executable in PATH
defineReplace(findexe) {
    return ( $$system(for %i in ($$1) do @echo. %~$PATH:i) )
}

# escape command to allow multiple lines in Makefile
defineReplace(mle) {
    return ( $$1$$escape_expand(\\n\\t) )
}

# check for gcc
GCC=$$findexe("gcc.exe")
GCCDIR=$$dirname(GCC)

isEmpty(GCC) {
    error("MinGW is not found in PATH")
}

# check for upx
UPX=$$findexe("upx.exe")

!isEmpty(UPX) {
    message("UPX is found, will pack the executable after linking")

    QMAKE_POST_LINK += $$mle($$UPX -9 \"$${OUT_PWD}/$(DESTDIR_TARGET)\")
} else {
    warning("UPX is not found, will not pack the executable")
}

# check for 7z
ZIP=$$findexe("7z.exe")

# check for signtool
SIGNTOOL=$$findexe("signtool.exe")
CERT=$${_PRO_FILE_PWD_}\\..\\$${TARGET}-certs\\cert.pfx

!isEmpty(SIGNTOOL):exists($$CERT) {
    message("Signtool and the certificate are found, will sign the $$TARGET executable")
} else {
    warning("Signtool or the certificate is not found, will not sign the $$TARGET executable")
}

SVNROOTORIG="https://traders-home-task-ng.googlecode.com/svn"
SVNROOT="http://traders-home-task-ng.googlecode.com/svn"
HTTPROOT="http://code.google.com/p/traders-home-task-ng"

DEFINES += SVNROOT=$$sprintf("\"\\\"%1\\\"\"", $$SVNROOT)
DEFINES += HTTPROOT=$$sprintf("\"\\\"%1\\\"\"", $$HTTPROOT)

tag.commands += $$mle(echo "$$VERSION"> "\"$${_PRO_FILE_PWD_}/THT-version.tag\"")
tag.commands += $$mle(svn -m "\"$$VERSION file tag\"" commit "\"$${_PRO_FILE_PWD_}/THT-version.tag\"")
tag.commands += $$mle(svn -m "\"$$VERSION tag\"" copy "\"$$SVNROOTORIG/trunk\"" "\"$$SVNROOTORIG/tags/$$VERSION\"")
QMAKE_EXTRA_TARGETS += tag

# files to copy to the distribution
IMAGEPLUGINS=qico4.dll qjpeg4.dll
SQLPLUGINS=qsqlite4.dll
QTLIBS=QtCore4.dll QtGui4.dll QtNetwork4.dll QtSql4.dll
MINGWLIBS=libgcc_s_dw2-1.dll libstdc++-6.dll mingwm10.dll
QMFILES=tht_ru.qm tht_uk.qm tht_en_US.qm
QTQMFILES=qt_ru.qm qt_uk.qm
LICENSES=LICENSE.txt LICENSE-LGPL.txt

# copy translations
QMAKE_POST_LINK += $$mle(lrelease $$_PRO_FILE_)
QMAKE_POST_LINK += $$mle(if not exist \"$${OUT_PWD}/$(DESTDIR_TARGET)/../translations\" mkdir \"$${OUT_PWD}/$(DESTDIR_TARGET)/../translations\")

for(qm, QMFILES) {
    QMAKE_POST_LINK += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\ts\\$$qm\" \"$${OUT_PWD}/$(DESTDIR_TARGET)/../translations\")
}

# copy database
QMAKE_POST_LINK += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\tickersdb\\tickers.sqlite\" \"$${OUT_PWD}/$(DESTDIR_TARGET)/..\")
QMAKE_POST_LINK += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\tickersdb\\tickers.sqlite.timestamp\" \"$${OUT_PWD}/$(DESTDIR_TARGET)/..\")

# sign
!isEmpty(SIGNTOOL):exists($$CERT) {
    QMAKE_POST_LINK += $$mle($$SIGNTOOL sign /d \"Trader\'s Home Task\" /du \"$$HTTPROOT\" /f \"$$CERT\" /t \"http://timestamp.verisign.com/scripts/timestamp.dll\" /v \"$${OUT_PWD}/$(DESTDIR_TARGET)\")
}

!isEmpty(ZIP) {
    message("7Z is found, will create custom dist targets")

    # source archive
    T="$${OUT_PWD}/tht-$$VERSION"
    distsrc.commands += $$mle(if exist \"$$T\" rd /S /Q \"$$T\")
    distsrc.commands += $$mle(svn export $$_PRO_FILE_PWD_ \"$$T\")
    distsrc.commands += $$mle($$ZIP a -r -tzip -mx=9 tht-$${VERSION}.zip \"$$T\")
    distsrc.commands += $$mle(rd /S /Q \"$$T\")

    QMAKE_EXTRA_TARGETS += distsrc

    # standalone binary
    T="$${OUT_PWD}/tht-standalone-$$VERSION"

    distbin.commands += $$mle(if exist \"$$T\" rd /S /Q \"$$T\")
    distbin.commands += $$mle(mkdir \"$$T\")
    distbin.commands += $$mle(mkdir \"$$T/plugins/imageformats\")
    distbin.commands += $$mle(mkdir \"$$T/plugins/sqldrivers\")
    distbin.commands += $$mle(mkdir \"$$T/translations\")
    distbin.commands += $$mle(copy /y \"$${OUT_PWD}/$(DESTDIR_TARGET)\" \"$$T\")

    for(ql, QTLIBS) {
        distbin.commands += $$mle(copy /y \"$$[QT_INSTALL_BINS]\\$$ql\" \"$$T\")
    }

    for(ml, MINGWLIBS) {
        distbin.commands += $$mle(copy /y \"$$GCCDIR\\$$ml\" \"$$T\")
    }

    for(ip, IMAGEPLUGINS) {
        distbin.commands += $$mle(copy /y \"$$[QT_INSTALL_PLUGINS]\\imageformats\\$$ip\" \"$$T/plugins/imageformats\")
    }

    for(ip, SQLPLUGINS) {
        distbin.commands += $$mle(copy /y \"$$[QT_INSTALL_PLUGINS]\\sqldrivers\\$$ip\" \"$$T/plugins/sqldrivers\")
    }

    for(qm, QMFILES) {
        distbin.commands += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\ts\\$$qm\" \"$$T/translations\")
    }

    for(qm, QTQMFILES) {
        distbin.commands += $$mle(copy /y \"$$[QT_INSTALL_TRANSLATIONS]\\$$qm\" \"$$T/translations\")
    }

    for(lc, LICENSES) {
        distbin.commands += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\$$lc\" \"$$T\")
    }

    distbin.commands += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\tickersdb\\tickers.sqlite\" \"$$T\")
    distbin.commands += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\tickersdb\\tickers.sqlite.timestamp\" \"$$T\")

    # compress
    distbin.commands += $$mle($$ZIP a -r -tzip -mx=9 tht-standalone-$${VERSION}.zip \"$$T\")
    distbin.commands += $$mle(rd /S /Q \"$$T\")

    QMAKE_EXTRA_TARGETS += distbin
} else {
    warning("7Z is not found, will not create custom dist targets")
}

# INNO setup
INNO=$$system(echo %ProgramFiles%)\\Inno Setup 5\\iscc.exe

exists($$INNO) {
    message("Inno Setup is found, will create a setup file in a custom dist target")

    LANGS=$$system(dir /B \"$$INNO\\..\\Languages\")

    ISS="tht-$${VERSION}.iss"

    iss.commands += $$mle(echo $${LITERAL_HASH}define MyAppName \"Trader\'s Home Task\" > $$ISS)
    iss.commands += $$mle(echo $${LITERAL_HASH}define MyAppPublisher \"Dmitry Baryshev\" >> $$ISS)
    iss.commands += $$mle(echo $${LITERAL_HASH}define MyAppURL \"$$HTTPROOT\" >> $$ISS)

    iss.commands += $$mle(echo [Setup] >> $$ISS)
    iss.commands += $$mle(echo AppId={{16AE5DDE-D073-4F5F-ABC3-11DD9FBF58E3} >> $$ISS)
    iss.commands += $$mle(echo AppName={$${LITERAL_HASH}MyAppName} >> $$ISS)
    iss.commands += $$mle(echo AppVersion=$$VERSION >> $$ISS)
    iss.commands += $$mle(echo AppPublisher={$${LITERAL_HASH}MyAppPublisher} >> $$ISS)
    iss.commands += $$mle(echo AppPublisherURL={$${LITERAL_HASH}MyAppURL} >> $$ISS)
    iss.commands += $$mle(echo AppSupportURL={$${LITERAL_HASH}MyAppURL} >> $$ISS)
    iss.commands += $$mle(echo AppUpdatesURL={$${LITERAL_HASH}MyAppURL} >> $$ISS)
    iss.commands += $$mle(echo DefaultDirName={pf}\\{$${LITERAL_HASH}MyAppName} >> $$ISS)
    iss.commands += $$mle(echo DefaultGroupName={$${LITERAL_HASH}MyAppName} >> $$ISS)
    iss.commands += $$mle(echo LicenseFile=$${_PRO_FILE_PWD_}\\LICENSE.rtf >> $$ISS)
    iss.commands += $$mle(echo OutputDir=. >> $$ISS)
    iss.commands += $$mle(echo OutputBaseFilename=tht-setup-$$VERSION >> $$ISS)
    iss.commands += $$mle(echo SetupIconFile=$${_PRO_FILE_PWD_}\\images\\chart.ico >> $$ISS)
    iss.commands += $$mle(echo Compression=lzma >> $$ISS)
    iss.commands += $$mle(echo SolidCompression=yes >> $$ISS)
    iss.commands += $$mle(echo UninstallDisplayName={$${LITERAL_HASH}MyAppName} >> $$ISS)
    iss.commands += $$mle(echo UninstallDisplayIcon={app}\\$${TARGET}.exe >> $$ISS)
    iss.commands += $$mle(echo MinVersion="0,5.1" >> $$ISS)

    !isEmpty(SIGNTOOL):exists($$CERT) {
        iss.commands += $$mle(echo SignTool=bps sign /d \$\$qTrader\'s Home Task\$\$q /du \$\$q$$HTTPROOT\$\$q /f \$\$q$$CERT\$\$q /t \$\$qhttp://timestamp.verisign.com/scripts/timestamp.dll\$\$q /v \$\$q\$\$f\$\$q >> $$ISS)
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

    iss.commands += $$mle(echo [Tasks] >> $$ISS)
    iss.commands += $$mle(echo Name: \"desktopicon\"; Description: \"{cm:CreateDesktopIcon}\"; GroupDescription: \"{cm:AdditionalIcons}\" >> $$ISS)
    iss.commands += $$mle(echo Name: \"quicklaunchicon\"; Description: \"{cm:CreateQuickLaunchIcon}\"; GroupDescription: \"{cm:AdditionalIcons}\"; OnlyBelowVersion: "0,6.1" >> $$ISS)

    iss.commands += $$mle(echo [Files] >> $$ISS)
    iss.commands += $$mle(echo Source: \"$${OUT_PWD}/$(DESTDIR_TARGET)\"; DestDir: \"{app}\"; Flags: ignoreversion >> $$ISS)
    iss.commands += $$mle(echo Source: \"$${_PRO_FILE_PWD_}\\tickersdb\\tickers.sqlite\"; DestDir: \"{app}\"; Flags: ignoreversion >> $$ISS)
    iss.commands += $$mle(echo Source: \"$${_PRO_FILE_PWD_}\\tickersdb\\tickers.sqlite.timestamp\"; DestDir: \"{app}\"; Flags: ignoreversion >> $$ISS)

    for(lc, LICENSES) {
        iss.commands += $$mle(echo Source: \"$${_PRO_FILE_PWD_}\\$$lc\"; DestDir: \"{app}\"; Flags: ignoreversion >> $$ISS)
    }

    for(qm, QMFILES) {
        iss.commands += $$mle(echo Source: \"$${_PRO_FILE_PWD_}\\ts\\$$qm\"; DestDir: \"{app}/translations\"; Flags: ignoreversion >> $$ISS)
    }

    for(qm, QTQMFILES) {
        iss.commands += $$mle(echo Source: \"$$[QT_INSTALL_TRANSLATIONS]\\$$qm\"; DestDir: \"{app}/translations\"; Flags: ignoreversion >> $$ISS)
    }

    for(ql, QTLIBS) {
        iss.commands += $$mle(echo Source: \"$$[QT_INSTALL_BINS]\\$$ql\"; DestDir: \"{app}\"; Flags: ignoreversion >> $$ISS)
    }

    for(ip, IMAGEPLUGINS) {
        iss.commands += $$mle(echo Source: \"$$[QT_INSTALL_PLUGINS]\\imageformats\\$$ip\"; DestDir: \"{app}\\plugins\\imageformats\"; Flags: ignoreversion >> $$ISS)
    }

    for(sp, SQLPLUGINS) {
        iss.commands += $$mle(echo Source: \"$$[QT_INSTALL_PLUGINS]\\sqldrivers\\$$sp\"; DestDir: \"{app}\\plugins\\sqldrivers\"; Flags: ignoreversion >> $$ISS)
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
