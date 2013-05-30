TARGET = THT-lib
TARGET_EXT = .dll
TEMPLATE = lib

INCLUDEPATH += qxt
DEPENDPATH += qxt

QT += core gui network sql
greaterThan(QT_MAJOR_VERSION, 4): QT *= widgets

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
    sqltools.cpp

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
    sqltools.h

FORMS += datamanagerbase.ui

TRANSLATIONS += ts/tht_lib_ru.ts \
                ts/tht_lib_uk.ts

RC_FILE = tht-lib.rc

include(THT-version.pri)
include(THT-common.pri)
