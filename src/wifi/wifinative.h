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

#ifndef WIFINATIVE_H
#define WIFINATIVE_H

#include <WiFi/wifiglobal.h>
#include <WiFi/wifi.h>
#include <WiFi/wifiinfo.h>
#include <WiFi/wifiscanresult.h>
#include <WiFi/wifinetwork.h>

class WiFiNativePrivate;
class WIFI_EXPORT WiFiNative : public QObject
{
    Q_OBJECT
public:
    explicit WiFiNative(QObject *parent = nullptr);

    WiFi::State wifiState() const;

    bool isWiFiEnabled() const;
    void setWiFiEnabled(bool enabled);

    bool isAutoScan() const;
    void setAutoScan(bool enabled);

    bool is5GHzBandSupported() const;
    bool isP2pSupported() const;

    WiFiInfo connectionInfo() const;
    WiFiScanResultList scanResults() const;
    WiFiNetworkList networks() const;

    static quint16 CalculateSignalLevel(int rssi, quint16 numLevels);

public slots:
    void pingSupplicant();
    void saveConfiguration();
    void startScan();

    int addNetwork(const WiFiNetwork &network);
    void selectNetwork(int networkId);
    void removeNetwork(int networkId);

signals:
    void wifiStateChanged();
    void isAutoScanChanged();
    void connectionInfoChanged();
    void scanResultFound(const WiFiScanResult &result);
    void scanResultUpdated(const WiFiScanResult &result);
    void scanResultLost(const WiFiScanResult &result);
    void networksChanged();

    void networkConnecting(int networkId);
    void networkAuthenticated(int networkId);
    void networkConnected(int networkId);
    void networkErrorOccurred(int networkId);

private:
    Q_DECLARE_PRIVATE(WiFiNative)

    Q_PRIVATE_SLOT(d_func(), void _q_updateInfoTimeout())
    Q_PRIVATE_SLOT(d_func(), void _q_autoScanTimeout())
    Q_PRIVATE_SLOT(d_func(), void _q_connNetTimeout())
};

#endif // WIFINATIVE_H
