/**
 ** This file is part of the WiFi project.
 ** Copyright 2019 张作深 <zhangzuoshen@hangsheng.com.cn>.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef WIFI_H
#define WIFI_H

#include <WiFi/wifiglobal.h>

QT_BEGIN_NAMESPACE

namespace WiFi
{
    static const int MIN_RSSI = -100;
    static const int MAX_RSSI = -55;

    enum State {
        StateDisabling,
        StateDisabled,
        StateEnabling,
        StateEnabled,
        StateUnknown
    };

    enum Auth {
        NoneOpen        = 0x00,
        NoneWEP         = 0x01,
        NoneWEPShared   = 0x02,
        IEEE8021X       = 0x04,
        WPA_PSK         = 0x08,
        WPA_EAP         = 0x10,
        WPA2_PSK        = 0x20,
        WPA2_EAP        = 0x40
    };
    Q_DECLARE_FLAGS(AuthFlags, Auth)
    Q_DECLARE_OPERATORS_FOR_FLAGS(AuthFlags)

    enum Encrytion {
        None    = 0x00,
        WEP     = 0x01,
        TKIP    = 0x02,
        CCMP    = 0x04
    };
    Q_DECLARE_FLAGS(EncrytionFlags, Encrytion)
    Q_DECLARE_OPERATORS_FOR_FLAGS(EncrytionFlags)

    enum DeviceType {
        DeviceUnknown,
        DevicePhone,
        DevicePC
    };

    WIFI_EXPORT QString toString(Auth auth);
    WIFI_EXPORT QString toString(AuthFlags auths);
    WIFI_EXPORT QString toString(Encrytion encr);
    WIFI_EXPORT QString toString(EncrytionFlags encrs);
}

QT_END_NAMESPACE

Q_DECLARE_METATYPE(WiFi::State)
Q_DECLARE_METATYPE(WiFi::Auth)
Q_DECLARE_METATYPE(WiFi::AuthFlags)
Q_DECLARE_METATYPE(WiFi::Encrytion)
Q_DECLARE_METATYPE(WiFi::EncrytionFlags)
Q_DECLARE_METATYPE(WiFi::DeviceType)

#endif // WIFI_H
