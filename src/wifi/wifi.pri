DEFINES += CONFIG_CTRL_IFACE

win32 {
    LIBS += -lws2_32 -static
    DEFINES += CONFIG_NATIVE_WINDOWS CONFIG_CTRL_IFACE_NAMED_PIPE
    SOURCES += $$PWD/../3rdparty/wpa_supplicant/src/utils/os_win32.c
} else:win32-g++ {
    # cross compilation to win32
    LIBS += -lws2_32 -static -mwindows
    DEFINES += CONFIG_NATIVE_WINDOWS CONFIG_CTRL_IFACE_NAMED_PIPE
    SOURCES += $$PWD/../3rdparty/wpa_supplicant/src/utils/os_win32.c
} else:win32-x-g++ {
    # cross compilation to win32
    LIBS += -lws2_32 -static -mwindows
    DEFINES += CONFIG_NATIVE_WINDOWS CONFIG_CTRL_IFACE_NAMED_PIPE
    DEFINES += _X86_
    SOURCES += $$PWD/../3rdparty/wpa_supplicant/src/utils/os_win32.c
} else {
    DEFINES += CONFIG_CTRL_IFACE_UNIX
    SOURCES += $$PWD/../3rdparty/wpa_supplicant/src/utils/os_unix.c \
               $$PWD/../3rdparty/wpa_supplicant/src/utils/common.c \
               $$PWD/../3rdparty/wpa_supplicant/src/common/wpa_ctrl.c
}

INCLUDEPATH += $$PWD/../3rdparty/wpa_supplicant/src \
               $$PWD/../3rdparty/wpa_supplicant/src/utils


HEADERS += \
    $$PWD/wifimacaddress.h \
    $$PWD/wifiscanresult.h \
    $$PWD/wifiinfo.h \
    $$PWD/wifi.h \
    $$PWD/wifinetwork.h \
    $$PWD/wifimanager.h \
    $$PWD/wifisupplicanttool_p.h \
    $$PWD/wifimanager_p.h

SOURCES += \
    $$PWD/wifimacaddress.cpp \
    $$PWD/wifiscanresult.cpp \
    $$PWD/wifiinfo.cpp \
    $$PWD/wifi.cpp \
    $$PWD/wifinetwork.cpp \
    $$PWD/wifimanager.cpp \
    $$PWD/wifisupplicanttool.cpp
