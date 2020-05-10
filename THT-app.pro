TARGET = THT
TEMPLATE = app

NVER1=2
NVER2=4
NVER3=0

QT += core gui network xml sql

INCLUDEPATH += qtsingleapplication qtdropbox kde
DEPENDPATH += qtsingleapplication qtdropbox kde

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
    updatechecker.cpp \
    listitem.cpp \
    tickerminiinput.cpp \
    tickersdatabaseupdater.cpp \
    tickerneighbors.cpp \
    linkpointmanager.cpp \
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
    numericlabel.cpp \
    thttools.cpp \
    linkedwindow.cpp \
    linkpointsession.cpp \
    linkpoint.cpp \
    kde/kcolorcombo.cpp \
    styledescriptionreader.cpp \
    idletimer.cpp

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
    updatechecker.h \
    listitem.h \
    tickerminiinput.h \
    tickersdatabaseupdater.h \
    tickerneighbors.h \
    linkpointmanager.h \
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
    numericlabel.h \
    thttools.h \
    linkedwindow.h \
    linkpointsession.h \
    linkpoint.h \
    thtsettings.h \
    kde/kcolorcombo.h \
    styledescriptionreader.h \
    defaultstyles.h \
    idletimer.h

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

include(THT-common.pri)

# network data
GITROOT_FOR_DOWNLOAD="https://github.com/smoked-herring/tht/raw/master"
HTTPROOT="https://github.com/smoked-herring/tht"
DOWNLOADROOT="https://github.com/smoked-herring/tht/releases"

DEFINES += GITROOT_FOR_DOWNLOAD=$$sprintf("\"\\\"%1\\\"\"", $$GITROOT_FOR_DOWNLOAD)
DEFINES += HTTPROOT=$$sprintf("\"\\\"%1\\\"\"", $$HTTPROOT)
DEFINES += DOWNLOADROOT=$$sprintf("\"\\\"%1\\\"\"", $$DOWNLOADROOT)

# copy database
QMAKE_POST_LINK += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\tickersdb\\tickers.sqlite\" \"$${OUT_PWD}/$(DESTDIR_TARGET)/..\")
QMAKE_POST_LINK += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\tickersdb\\tickers.sqlite.timestamp\" \"$${OUT_PWD}/$(DESTDIR_TARGET)/..\")
QMAKE_POST_LINK += $$mle(if not exist \"$${OUT_PWD}/$(DESTDIR_TARGET)/../styles\" xcopy \"$$replace(_PRO_FILE_PWD_, /, \\)\\styles\" \"$${OUT_PWD}/$(DESTDIR_TARGET)/../styles\" /s /q /y /i)
