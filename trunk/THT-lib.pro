TARGET = THT-lib
TARGET_EXT = .dll
TEMPLATE = lib

INCLUDEPATH += qxt
DEPENDPATH += qxt

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
    networkaccessdialog.cpp

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
    networkaccessdialog.h

FORMS += datamanagerbase.ui \
    networkaccessdialog.ui

RC_FILE = tht-lib.rc

include(THT-version.pri)
include(THT-common.pri)
