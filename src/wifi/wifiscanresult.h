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

QT_BEGIN_NAMESPACE

class WiFiMacAddress;
class WiFiScanResultPrivate;
class WIFI_EXPORT WiFiScanResult
{
public:
    WiFiScanResult();
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

    QString capabilities() const;
    void setCapabilities(const QString &flags);

    qint64 timestamp() const;
    void setTimestamp(qint64 microseconds);

protected:
    WiFiScanResultPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(WiFiScanResult)
};

#endif // WIFISCANRESULT_H
