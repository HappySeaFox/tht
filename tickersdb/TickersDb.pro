QT += core gui network sql webkit

TARGET = TickersDb
TEMPLATE = app

RUNNERNAME = $${TARGET}-run.bat
RUNNER = "\"$${_PRO_FILE_PWD_}\\$$RUNNERNAME\""

system(copy /y "\"$$[QT_INSTALL_BINS]\\qtenv2.bat\"" $$RUNNER)
system(echo. >> $$RUNNER)
system(echo hidecmd >> $$RUNNER)
system(echo. >> $$RUNNER)
#system(echo ":loop" >> $$RUNNER)
system(echo "\"$${OUT_PWD}\\release\\$${TARGET}.exe\"" >> $$RUNNER)
#system(echo "cmd /c \"ping 127.0.0.1 -n 3600 -w 1000 >nul\"" >> $$RUNNER)
#system(echo "goto loop" >> $$RUNNER)

DEFINES += TICKERS_DIR=$$sprintf("\"\\\"%1\\\"\"", $$_PRO_FILE_PWD_)

SOURCES += main.cpp\
        widget.cpp \
        networkaccess.cpp \
    csvreader.cpp

HEADERS  += widget.h \
        networkaccess.h \
    csvreader.h

FORMS    += widget.ui

RESOURCES += \
    tickersdb.qrc

RC_FILE = tickersdb.rc
