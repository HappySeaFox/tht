TARGET = THT-lib
TARGET_EXT = .dll
TEMPLATE = lib

NVER1=2
NVER2=0
NVER3=3

QT += core gui network sql

SOURCES += datamanagerbase.cpp \
    settings.cpp \
    plugin.cpp \
    tools.cpp \
    csvreader.cpp \
    networkaccess.cpp \
    pluginimportexport.cpp \
    qxt/qxtglobalshortcut_win.cpp \
    qxt/qxtglobalshortcut.cpp \
    qxt/qxtglobal.cpp \
    uppercasevalidator.cpp \
    remotedate.cpp \
    sqltools.cpp \
    hotkey.cpp \
    noeditordelegate.cpp \
    clickablelabel.cpp \
    networkaccessdialog.cpp \
    tickerinformationtooltip.cpp \
    tickerinformationfetcher.cpp \
    simplecrypt.cpp

HEADERS += datamanagerbase.h \
    settings.h \
    plugin.h \
    tools.h \
    csvreader.h \
    networkaccess.h \
    pluginimportexport.h \
    hotkey.h \
    qxt/qxtglobalshortcut_p.h \
    qxt/qxtglobalshortcut.h \
    qxt/qxtglobal.h \
    uppercasevalidator.h \
    remotedate.h \
    sqltools.h \
    noeditordelegate.h \
    clickablelabel.h \
    networkaccessdialog.h \
    tickerinformationtooltip.h \
    tickerinformationfetcher.h \
    simplecrypt.h

FORMS += datamanagerbase.ui \
    networkaccessdialog.ui

LIBS += -lUser32 -lAdvapi32

RC_FILE = tht-lib.rc

include(THT-common.pri)
