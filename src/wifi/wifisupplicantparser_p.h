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

#ifndef WIFISUPPLICANTPARSER_P_H
#define WIFISUPPLICANTPARSER_P_H

#include <WiFi/wifi.h>
#include <WiFi/wifimacaddress.h>
#include <WiFi/wifiinfo.h>
#include <WiFi/wifiscanresult.h>
#include <WiFi/wifinetwork.h>

#include <QtCore/qloggingcategory.h>

// in a header
Q_DECLARE_LOGGING_CATEGORY(logWPA)

QT_BEGIN_NAMESPACE

class WiFiSupplicantParser
{
public:
    WiFiSupplicantParser();

    WiFiInfo fromStatus(const QString &status) const;

    WiFiScanResult fromBSS(const QString &bss) const;

    WiFiNetworkList fromListNetworks(const QString &networks) const;

    WiFi::AuthFlags fromProtoKeyMgmt(const QString &proto,
                                     const QString &key_mgmt) const;

    WiFi::EncrytionFlags fromPairwise(const QString &pairwise) const;
};

QT_END_NAMESPACE

#endif // WIFISUPPLICANTPARSER_P_H
