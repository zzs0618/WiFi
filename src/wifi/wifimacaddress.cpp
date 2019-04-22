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

#include "wifimacaddress.h"

#ifndef QT_NO_DEBUG_STREAM
    #include <QDebug>
#endif

QT_BEGIN_NAMESPACE

class WiFiMacAddressPrivate
{
public:
    WiFiMacAddressPrivate();

    quint64 m_address;
};

/*!
    \class WiFiMacAddress
    \inmodule WiFi
    \brief 类 WiFiMacAddress 将一个物理地址分配给 WiFi 设备。
    \since 5.8

    该类以独立于平台和协议的方式保存 WiFi 地址。
*/

/*!
    \fn inline bool WiFiMacAddress::operator!=(const WiFiMacAddress &other) const


    将此 WiFi 地址与其他地址进行比较。

    如果 WiFi 地址不相等，返回 true ，否则返回 false 。
*/

static void registerWiFiMacAddressMetaType()
{
    static bool initDone = false;
    if (!initDone) {
        qRegisterMetaType<WiFiMacAddress>();
        initDone = true;
    }
}

/*!
    构造一个空的 WiFiMacAddress 对象。
*/
WiFiMacAddress::WiFiMacAddress() :
    d_ptr(new WiFiMacAddressPrivate)
{
    registerWiFiMacAddressMetaType();
}

/*!
    构造一个新的 WiFiMacAddress 对象并为其分配一个地址。
*/
WiFiMacAddress::WiFiMacAddress(quint64 address) :
    d_ptr(new WiFiMacAddressPrivate)
{
    registerWiFiMacAddressMetaType();

    Q_D(WiFiMacAddress);
    d->m_address = address;
}

/*!
    构造一个新的 WiFiMacAddress 对象并为其分配一个地址。

    地址 \a address 的格式可以是 XX:XX:XX:XX:XX:XX 或者 XXXXXXXXXXXX ，
    其中 X 是一个十六进制数字。
*/
WiFiMacAddress::WiFiMacAddress(const QString &address) :
    d_ptr(new WiFiMacAddressPrivate)
{
    registerWiFiMacAddressMetaType();

    Q_D(WiFiMacAddress);

    QString a = address;

    if (a.length() == 17) {
        a.remove(QLatin1Char(':'));
    }

    if (a.length() == 12) {
        bool ok;
        d->m_address = a.toULongLong(&ok, 16);
        if (!ok) {
            clear();
        }
    } else {
        d->m_address = 0;
    }
}

/*!
    构造一个新的 WiFiMacAddress 对象，该对象是 \a other 的副本。
*/
WiFiMacAddress::WiFiMacAddress(const WiFiMacAddress &other) :
    d_ptr(new WiFiMacAddressPrivate)
{
    *this = other;
}

/*!
    销毁该 WiFiMacAddress 对象。
*/
WiFiMacAddress::~WiFiMacAddress()
{
    delete d_ptr;
}

/*!
    将\a other分配到此 WiFiMacAddress 对象。
*/
WiFiMacAddress &WiFiMacAddress::operator=(const WiFiMacAddress &other)
{
    Q_D(WiFiMacAddress);

    d->m_address = other.d_func()->m_address;

    return *this;
}

/*!
    将 WiFi 地址设置为 00:00:00:00:00。
*/
void WiFiMacAddress::clear()
{
    Q_D(WiFiMacAddress);
    d->m_address = 0;
}

/*!
    如果 WiFi 地址为空，返回 true ，否则返回 false 。
*/
bool WiFiMacAddress::isNull() const
{
    Q_D(const WiFiMacAddress);
    return d->m_address == 0;
}

/*!
    如果 WiFi 地址小于 \a other ，则返回 true ，否则返回 false 。
*/
bool WiFiMacAddress::operator<(const WiFiMacAddress &other) const
{
    Q_D(const WiFiMacAddress);
    return d->m_address < other.d_func()->m_address;
}

/*!
    将此 WiFi 地址与 \a other 地址进行比较。

    如果两个 WiFi 地址相等，返回 true ，否则返回 false 。
*/
bool WiFiMacAddress::operator==(const WiFiMacAddress &other) const
{
    Q_D(const WiFiMacAddress);
    return d->m_address == other.d_func()->m_address;
}

/*!
    返回此 WiFi 地址为 quint64 类型格式。
*/
quint64 WiFiMacAddress::toUInt64() const
{
    Q_D(const WiFiMacAddress);
    return d->m_address;
}

/*!
    以格式化的字符串形式返回 WiFi 地址，XX:XX:XX:XX:XX:XX 。
*/
QString WiFiMacAddress::toString() const
{
    Q_D(const WiFiMacAddress);
    QString s(QStringLiteral("%1:%2:%3:%4:%5:%6"));

    for (int i = 5; i >= 0; --i) {
        const quint8 a = (d->m_address >> (i * 8)) & 0xff;
        s = s.arg(a, 2, 16, QLatin1Char('0'));
    }

    return s.toUpper();
}

WiFiMacAddressPrivate::WiFiMacAddressPrivate()
{
    m_address = 0;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, const WiFiMacAddress &address)
{
    debug << address.toString();
    return debug;
}
#endif

QT_END_NAMESPACE
