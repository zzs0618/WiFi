CXX_MODULE = wifi
TARGET  = quick_wifi
TARGETPATH = WiFi

QT += qml quick
QT_PRIVATE += core-private gui-private qml-private quick-private wifi-private

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0

SOURCES = \
    wifiplugin.cpp \
    qquickwifimanager.cpp \
    qquickwifisortfiltermodel.cpp

HEADERS = \
    qquickwifisortfiltermodel_p.h \
    qquickwifimanager_p.h

load(qml_plugin)
