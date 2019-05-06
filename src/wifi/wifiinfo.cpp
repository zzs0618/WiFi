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

#include "wifiinfo.h"
#include "wifimacaddress.h"

#include <QtCore/qjsondocument.h>


QT_BEGIN_NAMESPACE

class WiFiInfoPrivate
{
public:
    WiFiInfoPrivate();

    WiFiMacAddress address;
    WiFiMacAddress bssid;
    QString ssid;
    qint16 rssi;
    int frequency;
    QString ipAddress;
    int networkId;
    int rxLinkSpeedMbps;
    int txLinkSpeedMbps;
};

WiFiInfoPrivate::WiFiInfoPrivate() :
    rssi(-100),
    frequency(0),
    networkId(-1),
    rxLinkSpeedMbps(0),
    txLinkSpeedMbps(0)
{
}

/*!
    \class WiFiInfo
    \inmodule WiFi
    \brief 类 WiFiInfo 保存了关于 WiFi 连接的所有相关信息。
    \since 5.8

    该类描述任何处于活动状态或正在设置中的 WiFi 连接的状态。
*/


/*!
    构造一个空的 WiFiInfo 对象。
*/
WiFiInfo::WiFiInfo() :
    d_ptr(new WiFiInfoPrivate)
{
}

/*!
    构造一个 WiFiInfo 对象，它是 \a other 的副本。
*/
WiFiInfo::WiFiInfo(const WiFiInfo &other) :
    d_ptr(new WiFiInfoPrivate)
{
    *this = other;
}

/*!
    销毁 WiFiInfo 对象。
*/
WiFiInfo::~WiFiInfo()
{
    delete d_ptr;
}

/*!
    将\a other分配到此 WiFiInfo 对象。
*/
WiFiInfo &WiFiInfo::operator=(const WiFiInfo &other)
{
    Q_D(WiFiInfo);

    d->address = other.d_func()->address;
    d->bssid = other.d_func()->bssid;
    d->ssid = other.d_func()->ssid;
    d->rssi = other.d_func()->rssi;
    d->frequency = other.d_func()->frequency;
    d->ipAddress = other.d_func()->ipAddress;
    d->networkId = other.d_func()->networkId;
    d->rxLinkSpeedMbps = other.d_func()->rxLinkSpeedMbps;
    d->txLinkSpeedMbps = other.d_func()->txLinkSpeedMbps;

    return *this;
}

/*!
    将此 WiFiInfo 与 \a other 地址进行比较。

    如果两个 WiFiInfo 相等，返回 true ，否则返回 false 。
  */
bool WiFiInfo::operator==(const WiFiInfo &other) const
{
    Q_D(const WiFiInfo);

    return d->address == other.d_func()->address &&
           d->bssid == other.d_func()->bssid &&
           d->ssid == other.d_func()->ssid &&
           d->rssi == other.d_func()->rssi &&
           d->frequency == other.d_func()->frequency &&
           d->ipAddress == other.d_func()->ipAddress &&
           d->networkId == other.d_func()->networkId &&
           d->rxLinkSpeedMbps == other.d_func()->rxLinkSpeedMbps &&
           d->txLinkSpeedMbps == other.d_func()->txLinkSpeedMbps;
}

/*!
    如果此对象与 \a other 比较是否不同，不同则返回true，否则返回false。

    \sa operator==()
*/
bool WiFiInfo::operator!=(const WiFiInfo &other) const
{
    return !(*this == other);
}

/*!
    返回　WiFi 物理设备的地址。
*/
WiFiMacAddress WiFiInfo::macAddress() const
{
    Q_D(const WiFiInfo);
    return d->address;
}

void WiFiInfo::setMacAddress(const WiFiMacAddress &address)
{
    Q_D(WiFiInfo);
    d->address = address;
}

/*!
    返回当前访问点的基本服务集标识符(BSSID)。
*/
WiFiMacAddress WiFiInfo::bssid() const
{
    Q_D(const WiFiInfo);
    return d->bssid;
}

void WiFiInfo::setBSSID(const WiFiMacAddress &bssid)
{
    Q_D(WiFiInfo);
    d->bssid = bssid;
}

/*!
    返回当前802.11网络的服务集标识符(SSID)。
*/
QString WiFiInfo::ssid() const
{
    Q_D(const WiFiInfo);
    return d->ssid;
}

void WiFiInfo::setSSID(const QString &ssid)
{
    Q_D(WiFiInfo);
    d->ssid = ssid;
}

/*!
    返回当前802.11网络接收到的信号强度指示器，单位为dBm。
*/
qint16 WiFiInfo::rssi() const
{
    Q_D(const WiFiInfo);
    return d->rssi;
}

/*!
  设置 \a rssi 强度值，内部使用。
  */
void WiFiInfo::setRssi(qint16 rssi)
{
    Q_D(WiFiInfo);
    d->rssi = rssi;
}

/*!
    以 \a WiFiInfo::FrequencyUnits 频率单位返回当前频率。
*/
int WiFiInfo::frequency() const
{
    Q_D(const WiFiInfo);
    return d->frequency;
}

/*!
  设置 \a frequency 频率值，内部使用。
  */
void WiFiInfo::setFrequency(int frequency)
{
    Q_D(WiFiInfo);
    d->frequency = frequency;
}

/*!
    返回当前802.11网络被分配的 IP 地址。
*/
QString WiFiInfo::ipAddress() const
{
    Q_D(const WiFiInfo);
    return d->ipAddress;
}

/*!
  设置 \a ipAddress 地址值，内部使用。
  */
void WiFiInfo::setIpAddress(const QString &ipAddress)
{
    Q_D(WiFiInfo);
    d->ipAddress = ipAddress;
}

/*!
  每个配置的网络都有一个惟一的小整数ID，用于标识网络。
  */
int WiFiInfo::networkId() const
{
    Q_D(const WiFiInfo);
    return d->networkId;
}

/*!
  设置 \a id Network Id值，内部使用。
  */
void WiFiInfo::setNetworkId(int id)
{
    Q_D(WiFiInfo);
    d->networkId = id;
}

/*!
  返回当前接收链路速度(以Mbps为单位)。
  */
int WiFiInfo::rxLinkSpeed() const
{
    Q_D(const WiFiInfo);
    return d->rxLinkSpeedMbps;
}

/*!
  设置 \a speed 接收链路速度值(以Mbps为单位)，内部使用。
  */
void WiFiInfo::setRxLinkSpeed(int speed)
{
    Q_D(WiFiInfo);
    d->rxLinkSpeedMbps = speed;
}

/*!
  返回当前传输链路速度(以Mbps为单位)。
  */
int WiFiInfo::txLinkSpeed() const
{
    Q_D(const WiFiInfo);
    return d->txLinkSpeedMbps;
}

/*!
  设置 \a speed 传输链路速度值(以Mbps为单位)，内部使用。
  */
void WiFiInfo::setTxLinkSpeed(int speed)
{
    Q_D(WiFiInfo);
    d->txLinkSpeedMbps = speed;
}

QString WiFiInfo::toString() const
{
    Q_D(const WiFiInfo);
    QString s(QStringLiteral("BSSID = %1\n"
                             "SSID  = %2\n"
                             "RSSI  = %3\n"
                             "Freq  = %4 %5\n"
                             "IP    = %6\n"
                             "NetId = %7\n"
                             "MAC   = %8\n"));
    s = s.arg(d->bssid.toString());
    s = s.arg(d->ssid);
    s = s.arg(d->rssi);
    s = s.arg(d->frequency);
    s = s.arg(WiFiInfo::FrequencyUnits());
    s = s.arg(d->ipAddress);
    s = s.arg(d->networkId);
    s = s.arg(d->address.toString());

    return s;
}

QVariantMap WiFiInfo::toMap() const
{
    Q_D(const WiFiInfo);
    QVariantMap map;
    map[QLatin1String("macAddress")] = d->address.toString();
    map[QLatin1String("bssid")] = d->bssid.toString();
    map[QLatin1String("ssid")] = d->ssid;
    map[QLatin1String("rssi")] = d->rssi;
    map[QLatin1String("frequency")] = d->frequency;
    map[QLatin1String("ipAddress")] = d->ipAddress;
    map[QLatin1String("networkId")] = d->networkId;
    map[QLatin1String("rxLinkSpeed")] = d->rxLinkSpeedMbps;
    map[QLatin1String("txLinkSpeed")] = d->txLinkSpeedMbps;
    return map;
}

QByteArray WiFiInfo::toJson() const
{
    QJsonDocument doc = QJsonDocument::fromVariant(toMap());
    return doc.toJson(QJsonDocument::Compact);
}

WiFiInfo WiFiInfo::fromMap(const QVariantMap &map)
{
    if(map.isEmpty()) {
        qCritical() << "WiFiInfo::fromMap. Error at: map is Empty." ;
        return WiFiInfo();
    }
    WiFiInfo info;
    info.setMacAddress(WiFiMacAddress(map[QLatin1String("macAddress")].toString()));
    info.setBSSID(WiFiMacAddress(map[QLatin1String("bssid")].toString()));
    info.setSSID(map[QLatin1String("ssid")].toString());
    info.setRssi(map[QLatin1String("rssi")].toInt());
    info.setFrequency(map[QLatin1String("frequency")].toInt());
    info.setIpAddress(map[QLatin1String("ipAddress")].toString());
    info.setNetworkId(map[QLatin1String("networkId")].toInt());
    info.setRxLinkSpeed(map[QLatin1String("rxLinkSpeed")].toInt());
    info.setTxLinkSpeed(map[QLatin1String("txLinkSpeed")].toInt());
    return info;
}

WiFiInfo WiFiInfo::fromJson(const QByteArray &json)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(json, &parseError);
    if (parseError.error) {
        qCritical() << "WiFiInfo::fromJson. Error at:" << parseError.offset
                    << parseError.errorString();
        return WiFiInfo();
    }
    return WiFiInfo::fromMap(doc.toVariant().toMap());
}

QString WiFiInfo::FrequencyUnits()
{
    return QStringLiteral("MHz");
}

QT_END_NAMESPACE
