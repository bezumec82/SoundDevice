#-------------------------------------------------
#
# Project created by QtCreator 2019-03-29T15:04:37
#
#-------------------------------------------------

QT       += core
QT       -= gui
QT       += network
QT       += multimedia

TARGET = RTP_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
unix:QMAKE_CXXFLAGS += -std=c++11 -fpermissive
unix:QMAKE_CXXFLAGS_WARN_ON += -Wno-unknown-pragmas

BUILDDIR = $$PWD/tmp
OBJECTS_DIR = $${BUILDDIR}
MOC_DIR = $${BUILDDIR}
RCC_DIR = $${BUILDDIR}
UI_DIR = $${BUILDDIR}
CONFIG(debug, debug|release) {
    DESTDIR = debug
} else {
    DESTDIR = release
}

SOURCES += main.cpp
HEADERS += \
    main.h

unix:LIBS +=-lm
unix:LIBS +=-lnsl
unix:LIBS +=-lrt
unix:LIBS +=-lpthread
unix:LIBS +=-lasound
unix:LIBS +=-lcrypto
unix:LIBS +=-lssl
unix:LIBS +=-luuid

target.path = /tmp
INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Terminal/Server/protocols/rtp/lib/jrtplib-3.11.1/src/release/ -ljrtp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Terminal/Server/protocols/rtp/lib/jrtplib-3.11.1/src/debug/ -ljrtp
else:unix: LIBS += -L$$PWD/../../Terminal/Server/protocols/rtp/lib/jrtplib-3.11.1/src/ -ljrtp

INCLUDEPATH += $$PWD/../../Terminal/Server/protocols/rtp/lib/jrtplib-3.11.1/src
DEPENDPATH += $$PWD/../../Terminal/Server/protocols/rtp/lib/jrtplib-3.11.1/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Terminal/Server/protocols/rtp/lib/jrtplib-3.11.1/src/release/libjrtp.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Terminal/Server/protocols/rtp/lib/jrtplib-3.11.1/src/debug/libjrtp.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Terminal/Server/protocols/rtp/lib/jrtplib-3.11.1/src/release/jrtp.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Terminal/Server/protocols/rtp/lib/jrtplib-3.11.1/src/debug/jrtp.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../Terminal/Server/protocols/rtp/lib/jrtplib-3.11.1/src/libjrtp.a
