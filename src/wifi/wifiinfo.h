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

#ifndef WIFIINFO_H
#define WIFIINFO_H

#include <WiFi/wifiglobal.h>

QT_BEGIN_NAMESPACE

class WiFiMacAddress;
class WiFiInfoPrivate;
class WIFI_EXPORT WiFiInfo
{
public:
    WiFiInfo();
    WiFiInfo(const WiFiMacAddress &bssid, const QString &ssid);
    WiFiInfo(const WiFiInfo &other);
    ~WiFiInfo();

    bool isValid() const;
    bool isCached() const;

    void setCached(bool cached);

    WiFiInfo &operator=(const WiFiInfo &other);
    bool operator==(const WiFiInfo &other) const;
    bool operator!=(const WiFiInfo &other) const;

    WiFiMacAddress bssid() const;
    QString ssid() const;

    qint16 rssi() const;
    void setRssi(qint16 rssi);

    int frequency() const;
    void setFrequency(int frequency);

    QString ipAddress() const;
    void setIpAddress(const QString &ipAddress);

    int networkId() const;
    void setNetworkId(int id);

    int rxLinkSpeedMbps() const;
    void setRxLinkSpeedMbps(int speed);

    int txLinkSpeedMbps() const;
    void setTxLinkSpeedMbps(int speed);

    QString toString() const;
    QVariantMap toMap() const;
    QByteArray toJson() const;

    static WiFiInfo fromMap(const QVariantMap &map);
    static WiFiInfo fromJson(const QByteArray &json);

    static QString FrequencyUnits();

protected:
    WiFiInfoPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(WiFiInfo)
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(WiFiInfo)

#endif // WIFIINFO_H
