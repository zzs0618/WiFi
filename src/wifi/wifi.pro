TARGET = WiFi
MODULE = wifi

CONFIG += git_build

DEFINES += QT_BUILD_WIFI_LIB

QT += quick network dbus
QT_PRIVATE += core-private gui-private qml-private quick-private

DEFINES += QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII QT_NO_CAST_FROM_BYTEARRAY QT_NO_URL_CAST_FROM_STRING

DBUS_ADAPTORS += wifi.native.station.xml
DBUS_INTERFACES += wifi.native.station.xml
DBUS_ADAPTORS += wifi.native.peers.xml
DBUS_INTERFACES += wifi.native.peers.xml

HEADERS += \
    $$PWD/wifiglobal_p.h \
    $$PWD/wifiglobal.h

SOURCES += \
    $$PWD/wifiglobal.cpp

include(wifi.pri)

load(qt_module)
