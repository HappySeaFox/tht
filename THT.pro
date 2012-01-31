#-------------------------------------------------
#
# Project created by QtCreator 2012-01-29T00:47:02
#
#-------------------------------------------------

QT += core gui network

TARGET = THT
TEMPLATE = app

INCLUDEPATH += . qtsingleapplication
DEPENDPATH += . qtsingleapplication

# require at least Windows 2000
DEFINES += _WIN32_WINNT=0x0500 WINVER=0x0500

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
    qtsingleapplication/qtlocalpeer.cpp

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
    qtsingleapplication/qtlocalpeer.h

FORMS += tht.ui \
    list.ui \
    options.ui \
    about.ui

RESOURCES += \
    tht.qrc

RC_FILE = tht.rc

OTHER_FILES += \
    tht.rc

TRANSLATIONS += ts/ru.ts
