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

#ifndef WIFIMACADDRESS_H
#define WIFIMACADDRESS_H

#include <WiFi/wifiglobal.h>

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QMetaType>

QT_BEGIN_NAMESPACE

class WiFiMacAddressPrivate;

class WIFI_EXPORT WiFiMacAddress
{
public:
    WiFiMacAddress();
    explicit WiFiMacAddress(quint64 address);
    explicit WiFiMacAddress(const QString &address);
    WiFiMacAddress(const WiFiMacAddress &other);
    ~WiFiMacAddress();
    WiFiMacAddress &operator=(const WiFiMacAddress &other);
    bool isNull() const;
    void clear();
    bool operator<(const WiFiMacAddress &other) const;
    bool operator==(const WiFiMacAddress &other) const;
    inline bool operator!=(const WiFiMacAddress &other) const
    {
        return !operator==(other);
    }
    quint64 toUInt64() const;
    QString toString() const;

private:
    Q_DECLARE_PRIVATE(WiFiMacAddress)
    WiFiMacAddressPrivate *d_ptr;
};

#ifndef QT_NO_DEBUG_STREAM
    WIFI_EXPORT QDebug operator<<(QDebug, const WiFiMacAddress &address);
#endif

QT_END_NAMESPACE

Q_DECLARE_METATYPE(WiFiMacAddress)

#endif // WIFIMACADDRESS_H
