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

#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <WiFi/wifiglobal.h>
#include <WiFi/wifi.h>
#include <WiFi/wifiinfo.h>
#include <WiFi/wifiscanresult.h>
#include <WiFi/wifinetwork.h>

class WiFiManagerPrivate;
class WIFI_EXPORT WiFiManager : public QObject
{
    Q_OBJECT
public:
    explicit WiFiManager(QObject *parent = nullptr);

    bool isWiFiServiced() const;

    bool isWiFiEnabled() const;
    void setWiFiEnabled(bool enabled);

    bool isWiFiAutoScan() const;
    void setWiFiAutoScan(bool autoScan);

    bool is5GHzBandSupported() const;
    bool isP2pSupported() const;

    WiFiInfo connectionInfo() const;
    WiFiScanResultList scanResults() const;
    WiFiNetworkList networks() const;

    static quint16 CalculateSignalLevel(int rssi, quint16 numLevels);

public slots:
    int addNetwork(const WiFiNetwork &network);
    void selectNetwork(int networkId);
    void removeNetwork(int networkId);

signals:
    void isWiFiServicedChanged();
    void isWiFiEnabledChanged();
    void isWiFiAutoScanChanged();
    void connectionInfoChanged();
    void scanResultFound(const WiFiScanResult &result);
    void scanResultUpdated(const WiFiScanResult &result);
    void scanResultLost(const WiFiScanResult &result);
    void scanResultsChanged();
    void networksChanged();

    void networkConnecting(int networkId);
    void networkAuthenticated(int networkId);
    void networkConnected(int networkId);
    void networkErrorOccurred(int networkId);

private:
    Q_DECLARE_PRIVATE(WiFiManager)
};

#endif // WIFIMANAGER_H
