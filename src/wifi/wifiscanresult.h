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

#ifndef WIFISCANRESULT_H
#define WIFISCANRESULT_H

#include <WiFi/wifiglobal.h>
#include <WiFi/wifi.h>

QT_BEGIN_NAMESPACE

class WiFiMacAddress;
class WiFiScanResultPrivate;
class WIFI_EXPORT WiFiScanResult
{
public:
    WiFiScanResult();
    WiFiScanResult(const QString &bssid, const QString &ssid);
    WiFiScanResult(const WiFiMacAddress &bssid, const QString &ssid);
    WiFiScanResult(const WiFiScanResult &other);
    ~WiFiScanResult();

    bool isValid() const;
    bool isCached() const;

    void setCached(bool cached);

    WiFiScanResult &operator=(const WiFiScanResult &other);
    bool operator==(const WiFiScanResult &other) const;
    bool operator!=(const WiFiScanResult &other) const;

    WiFiMacAddress bssid() const;
    QString ssid() const;

    qint16 rssi() const;
    void setRssi(qint16 rssi);

    int frequency() const;
    void setFrequency(int frequency);
    /*
        WiFi::AuthFlags authFlags() const;
        void setAuthFlags(WiFi::AuthFlags auths);

        WiFi::EncrytionFlags encrFlags() const;
        void setEncrFlags(WiFi::EncrytionFlags encrs);
    */
    QString flags() const;
    void setFlags(const QString &flags);

    qint64 timestamp() const;
    void setTimestamp(qint64 microseconds);

    int networkId() const;
    void setNetworkId(int id);

    bool is24GHz() const;
    bool is5GHz() const;

    QString toString() const;
    QVariantMap toMap() const;
    QByteArray toJson() const;

    static WiFiScanResult fromMap(const QVariantMap &map);
    static WiFiScanResult fromJson(const QByteArray &json);

protected:
    WiFiScanResultPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(WiFiScanResult)
};

class WiFiScanResultList : public QList<WiFiScanResult>
{
public:
    QVariantList toMapList() const;
    QByteArray toJson() const;

    static WiFiScanResultList fromMapList(const QVariantList &mapList);
    static WiFiScanResultList fromJson(const QByteArray &json);
};

QT_END_NAMESPACE

#endif // WIFISCANRESULT_H
