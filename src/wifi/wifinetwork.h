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

#ifndef WIFINETWORK_H
#define WIFINETWORK_H


#include <WiFi/wifiglobal.h>
#include <WiFi/wifi.h>

QT_BEGIN_NAMESPACE

class WiFiMacAddress;
class WiFiNetworkPrivate;
class WIFI_EXPORT WiFiNetwork
{
public:
    WiFiNetwork();
    WiFiNetwork(int id, const QString &ssid);
    WiFiNetwork(const WiFiNetwork &other);
    ~WiFiNetwork();

    bool isValid() const;
    bool isCached() const;

    void setCached(bool cached);

    WiFiNetwork &operator=(const WiFiNetwork &other);
    bool operator==(const WiFiNetwork &other) const;
    bool operator!=(const WiFiNetwork &other) const;

    int networkId() const;
    QString ssid() const;

    WiFiMacAddress bssid() const;
    void setBSSID(const WiFiMacAddress &bssid);

    WiFi::AuthFlags authFlags() const;
    void setAuthFlags(WiFi::AuthFlags auths);

    WiFi::EncrytionFlags encrFlags() const;
    void setEncrFlags(WiFi::EncrytionFlags encrs);

    QString preSharedKey() const;
    void setPreSharedKey(const QString &psk);

    QString toString() const;
    QVariantMap toMap() const;
    QByteArray toJson() const;

    static WiFiNetwork fromMap(const QVariantMap &map);
    static WiFiNetwork fromJson(const QByteArray &json);

protected:
    WiFiNetworkPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(WiFiNetwork)
};

class WiFiNetworkList : public QList<WiFiNetwork>
{

};

QT_END_NAMESPACE

#endif // WIFINETWORK_H
