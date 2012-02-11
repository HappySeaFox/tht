#-------------------------------------------------
#
# Project created by QtCreator 2012-01-29T00:47:02
#
#-------------------------------------------------

QT += core gui network script

TARGET = THT
TEMPLATE = app

DEFINES += NVER1=0
DEFINES += NVER2=6
DEFINES += NVER3=0

DEFINES += TARGET=$$TARGET

QTPLUGIN += qico

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
    qtsingleapplication/qtsinglecoreapplication.cpp \
    qtsingleapplication/qtsingleapplication.cpp \
    qtsingleapplication/qtlockedfile_win.cpp \
    qtsingleapplication/qtlockedfile_unix.cpp \
    qtsingleapplication/qtlockedfile.cpp \
    qtsingleapplication/qtlocalpeer.cpp \
    regionselect.cpp \
    savescreenshot.cpp \
    qxt/qxtglobalshortcut_win.cpp \
    qxt/qxtglobalshortcut.cpp \
    qxt/qxtglobal.cpp \
    target.cpp \
    tickerinput.cpp \
    tickerinformationtooltip.cpp

HEADERS += tht.h \
    list.h \
    options.h \
    about.h \
    settings.h \
    qtsingleapplication/qtsinglecoreapplication.h \
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
    tickerinformationtooltip.h

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
    LICENSE.txt

TRANSLATIONS += ts/ru.ts
