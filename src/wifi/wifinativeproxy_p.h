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

#ifndef WIFINATIVEPROXY_P_H
#define WIFINATIVEPROXY_P_H

#include <QtCore/qobject.h>
#include <WiFi/wifi.h>
#include <WiFi/wifiinfo.h>
#include <WiFi/wifiscanresult.h>
#include <WiFi/wifinetwork.h>


class WiFiNativeProxyPrivate;
class WiFiNativeProxy : public QObject
{
    Q_OBJECT
public:
    explicit WiFiNativeProxy(QObject *parent = nullptr);

    bool isWiFiEnabled() const;
    void setWiFiEnabled(bool enabled);

    WiFiInfo connectionInfo() const;
    WiFiScanResultList scanResults() const;
    WiFiNetworkList networks() const;

public slots:
    void addNetwork(const WiFiNetwork &network);

signals:
    void isWiFiEnabledChanged();
    void connectionInfoChanged();
    void scanResultsChanged();
    void networksChanged();

private:
    Q_DECLARE_PRIVATE(WiFiNativeProxy)
};

#endif // WIFINATIVEPROXY_P_H