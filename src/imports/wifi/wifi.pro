CXX_MODULE = wifi
TARGET  = quick_wifi
TARGETPATH = WiFi

QT += qml quick
QT_PRIVATE += core-private gui-private qml-private quick-private wifi-private

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0

load(qml_plugin)

SOURCES = \
    wifiplugin.cpp \
    qquickwifimanager.cpp \
    qquickwifisortfiltermodel.cpp \
    qquickwifiscanresultmodel.cpp

HEADERS = \
    qquickwifimanager_p.h \
    qquickwifisortfiltermodel_p.h \
    qquickwifiscanresultmodel_p.h
