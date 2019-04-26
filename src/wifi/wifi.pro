TARGET = WiFi
MODULE = wifi

CONFIG += git_build

DEFINES += QT_BUILD_WIFI_LIB

QT += quick network
QT_PRIVATE += core-private gui-private qml-private quick-private

DEFINES += QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII QT_NO_CAST_FROM_BYTEARRAY QT_NO_URL_CAST_FROM_STRING

HEADERS += \
    $$PWD/wifiglobal_p.h \
    $$PWD/wifiglobal.h \
    wifisupplicantparser_p.h

SOURCES += \
    wifiglobal.cpp \
    wifisupplicantparser.cpp

include(wifi.pri)
load(qt_module)
