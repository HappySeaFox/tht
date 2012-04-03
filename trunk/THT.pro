#-------------------------------------------------
#
# Project created by QtCreator 2012-01-29T00:47:02
#
#-------------------------------------------------

QT += core gui network script

TARGET = THT
TEMPLATE = app

!win32 {
    error("$$TARGET requires Windows platform")
}

# THT version
NVER1=0
NVER2=7
NVER3=5

VERSION=$$sprintf("%1.%2.%3", $$NVER1, $$NVER2, $$NVER3)

DEFINES += NVER1=$$NVER1
DEFINES += NVER2=$$NVER2
DEFINES += NVER3=$$NVER3
DEFINES += NVER_STRING=$$sprintf("\"\\\"%1\\\"\"", $$VERSION)

DEFINES += TARGET=$$TARGET

INCLUDEPATH += . qtsingleapplication qxt
DEPENDPATH += . qtsingleapplication qxt

# require at least Windows XP
DEFINES += _WIN32_WINNT=0x0501 WINVER=0x0501

SOURCES += main.cpp\
    tht.cpp \
    list.cpp \
    options.cpp \
    about.cpp \
    settings.cpp \
    qtsingleapplication/qtsingleapplication.cpp \
    qtsingleapplication/qtlockedfile_win.cpp \
    qtsingleapplication/qtlockedfile.cpp \
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
    listitem.cpp

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
    listitem.h

FORMS += tht.ui \
    list.ui \
    options.ui \
    about.ui \
    savescreenshot.ui \
    tickerinput.ui

RESOURCES += \
    tht.qrc

LIBS += -lpsapi

RC_FILE = tht.rc

OTHER_FILES += \
    tht.rc \
    README.txt \
    LICENSE.txt \
    THT-version.tag

TRANSLATIONS += ts/ru.ts ts/uk.ts

QMAKE_PRE_LINK = lrelease $$_PRO_FILE_

# search an executable in PATH
defineReplace(findexe) {
    return ( $$system(for %i in ($$1) do @echo. %~$PATH:i) )
}

# escape command to allow multiple lines in Makefile
defineReplace(mle) {
    return ( $$1$$escape_expand(\\n\\t) )
}

# check for perl
PERL=$$findexe("perl.exe")

!isEmpty(PERL) {
    SVNROOT=$$system(svn info | perl -ne '\"if ($_ =~ /^Repository Root:(.*)/) {print $1;}\"')

    isEmpty(SVNROOT) {
        warning("Cannot determine the repository root")
    } else {
        message("Repository root: $$SVNROOT")

        tag.commands += $$mle(echo "$$VERSION"> "\"$${_PRO_FILE_PWD_}/THT-version.tag\"")
        tag.commands += $$mle(svn -m "\"$$VERSION file tag\"" commit "\"$${_PRO_FILE_PWD_}/THT-version.tag\"")
        tag.commands += $$mle(svn -m "\"$$VERSION tag\"" copy "\"$$SVNROOT/trunk\"" "\"$$SVNROOT/tags/$$VERSION\"")
        QMAKE_EXTRA_TARGETS += tag
    }
}

# check for upx
UPX=$$findexe("upx.exe")

!isEmpty(UPX) {
    message("UPX is found, will pack the executable after linking")
    QMAKE_POST_LINK = $$UPX -9 $${OUT_PWD}/$(DESTDIR_TARGET) # undocumented feature
} else {
    warning("UPX is not found, will not pack the executable")
}

ZIP=$$findexe("7z.exe")
GCC=$$findexe("gcc.exe")

!isEmpty(ZIP):!isEmpty(GCC) {
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
    G=$$dirname(GCC)

    # files to copy
    IMAGEPLUGINS=qgif4.dll qico4.dll qjpeg4.dll qtga4.dll qtiff4.dll
    QTLIBS=QtCore4.dll QtGui4.dll QtNetwork4.dll QtScript4.dll
    MINGWLIBS=libgcc_s_dw2-1.dll libstdc++-6.dll mingwm10.dll
    QMFILES=ru.qm uk.qm qt_ru.qm qt_uk.qm
    LICENSES=LICENSE.txt LICENSE-LGPL.txt

    distbin.commands += $$mle(if exist \"$$T\" rd /S /Q \"$$T\")
    distbin.commands += $$mle(mkdir \"$$T\")
    distbin.commands += $$mle(mkdir \"$$T/imageformats\")
    distbin.commands += $$mle(mkdir \"$$T/translations\")
    distbin.commands += $$mle(copy /y \"$${OUT_PWD}/$(DESTDIR_TARGET)\" \"$$T\")

    for(ql, QTLIBS) {
        distbin.commands += $$mle(copy /y \"$$[QT_INSTALL_BINS]\\$$ql\" \"$$T\")
    }

    for(ml, MINGWLIBS) {
        distbin.commands += $$mle(copy /y \"$$G\\$$ml\" \"$$T\")
    }

    for(ip, IMAGEPLUGINS) {
        distbin.commands += $$mle(copy /y \"$$[QT_INSTALL_PLUGINS]\\imageformats\\$$ip\" \"$$T/imageformats\")
    }

    for(qm, QMFILES) {
        distbin.commands += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\ts\\$$qm\" \"$$T/translations\")
    }

    for(lc, LICENSES) {
        distbin.commands += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\$$lc\" \"$$T\")
    }

    distbin.commands += $$mle($$ZIP a -r -tzip -mx=9 tht-standalone-$${VERSION}.zip \"$$T\")
    distbin.commands += $$mle(rd /S /Q \"$$T\")

    QMAKE_EXTRA_TARGETS += distbin
} else {
    warning("7Z is not found, will not create custom dist targets")
}
