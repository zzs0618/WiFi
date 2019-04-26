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

#include "wifiscanresult.h"
#include "wifimacaddress.h"

#include <QtCore/qjsondocument.h>


QT_BEGIN_NAMESPACE

class WiFiScanResultPrivate
{
public:
    WiFiScanResultPrivate();

    bool valid;
    bool cached;

    WiFiMacAddress bssid;
    QString ssid;
    qint16 rssi;
    int frequency;
    WiFi::AuthFlags authFlags;
    WiFi::EncrytionFlags encrFlags;
    QString flags;
    qint64 timestamp;
    int networkId;
};

WiFiScanResultPrivate::WiFiScanResultPrivate() :
    valid(false),
    cached(false),
    rssi(-100),
    frequency(0),
    authFlags(WiFi::NoneOpen),
    encrFlags(WiFi::None),
    timestamp(0),
    networkId(-1)
{
}

/*!
    \class WiFiScanResult
    \inmodule WiFi
    \brief 类 WiFiScanResult 保存了测到的接入点的相关信息。
    \since 5.8

    描述有关检测到的接入点的信息。
*/


/*!
    构造一个无效的 WiFiScanResult 对象。
*/
WiFiScanResult::WiFiScanResult() :
    d_ptr(new WiFiScanResultPrivate)
{

}

/*!
    构造一个 WiFiScanResult 对象，该对象具有 BSSID 和 SSID 。
*/
WiFiScanResult::WiFiScanResult(const QString &bssid, const QString &ssid)
    : WiFiScanResult(WiFiMacAddress(bssid), ssid)
{
}

/*!
    构造一个 WiFiScanResult 对象，该对象具有 BSSID 和 SSID 。
*/
WiFiScanResult::WiFiScanResult(const WiFiMacAddress &bssid,
                               const QString &ssid) :
    d_ptr(new WiFiScanResultPrivate)
{
    Q_D(WiFiScanResult);

    d->bssid = bssid;
    d->ssid = ssid;
    d->valid = true;
    d->cached = false;
    d->rssi = -100;
}

/*!
    构造一个 WiFiScanResult 对象，它是 \a other 的副本。
*/
WiFiScanResult::WiFiScanResult(const WiFiScanResult &other) :
    d_ptr(new WiFiScanResultPrivate)
{
    *this = other;
}

/*!
    销毁 WiFiScanResult 对象。
*/
WiFiScanResult::~WiFiScanResult()
{
    delete d_ptr;
}

/*!
    如果 WiFiScanResult 对象有效，则返回 true， 否则返回 false 。
*/
bool WiFiScanResult::isValid() const
{
    Q_D(const WiFiScanResult);
    return d->valid;
}

/*!
     如果从缓存的数据创建 WiFiScanResult 对象，则返回true。
 */
bool WiFiScanResult::isCached() const
{
    Q_D(const WiFiScanResult);
    return d->cached;
}

/*!
    如果从缓存的数据创建 WiFiScanResult 对象，则系统将使用它设置 \a cached 缓存标志。
    缓存信息可能不如从活动设备读取的数据准确。
  */
void WiFiScanResult::setCached(bool cached)
{
    Q_D(WiFiScanResult);
    d->cached = cached;
}

/*!
    将\a other分配到此 WiFiScanResult 对象。
*/
WiFiScanResult &WiFiScanResult::operator=(const WiFiScanResult &other)
{
    Q_D(WiFiScanResult);

    d->bssid = other.d_func()->bssid;
    d->ssid = other.d_func()->ssid;
    d->valid = other.d_func()->valid;
    d->cached = other.d_func()->cached;
    d->rssi = other.d_func()->rssi;
    d->frequency = other.d_func()->frequency;
    d->authFlags = other.d_func()->authFlags;
    d->encrFlags = other.d_func()->encrFlags;
    d->flags = other.d_func()->flags;
    d->timestamp = other.d_func()->timestamp;
    d->networkId = other.d_func()->networkId;

    return *this;
}

/*!
    将此 WiFiScanResult 与 \a other 地址进行比较。

    如果两个 WiFiScanResult 相等，返回 true ，否则返回 false 。
  */
bool WiFiScanResult::operator==(const WiFiScanResult &other) const
{
    Q_D(const WiFiScanResult);

    return d->bssid == other.d_func()->bssid;
}

/*!
    如果此对象与 \a other 比较是否不同，不同则返回true，否则返回false。

    \sa operator==()
*/
bool WiFiScanResult::operator!=(const WiFiScanResult &other) const
{
    return !(*this == other);
}

/*!
    返回接入点的地址。
*/
WiFiMacAddress WiFiScanResult::bssid() const
{
    Q_D(const WiFiScanResult);
    return d->bssid;
}

/*!
    返回接入点的网络名称。
*/
QString WiFiScanResult::ssid() const
{
    Q_D(const WiFiScanResult);
    return d->ssid;
}

/*!
    返回接入点的信号强度指示器，单位为dBm。
*/
qint16 WiFiScanResult::rssi() const
{
    Q_D(const WiFiScanResult);
    return d->rssi;
}

/*!
  设置 \a rssi 强度值，内部使用。
  */
void WiFiScanResult::setRssi(qint16 rssi)
{
    Q_D(WiFiScanResult);
    d->rssi = rssi;
}

/*!
    以 MHZ 频率单位返回接入点的频率。
*/
int WiFiScanResult::frequency() const
{
    Q_D(const WiFiScanResult);
    return d->frequency;
}

/*!
  设置 \a frequency 频率值，内部使用。
  */
void WiFiScanResult::setFrequency(int frequency)
{
    Q_D(WiFiScanResult);
    d->frequency = frequency;
}

/*!
    返回描述访问点支持的身份验证、密钥管理和加密方案。
*/
//WiFi::AuthFlags WiFiScanResult::authFlags() const
//{
//    Q_D(const WiFiScanResult);
//    return d->authFlags;
//}

/*!
  设置 \a frequency 频率值，内部使用。
  */
//void WiFiScanResult::setAuthFlags(WiFi::AuthFlags auths)
//{
//    Q_D(WiFiScanResult);
//    d->authFlags = auths;
//}

/*!
    返回描述访问点支持的身份验证、密钥管理和加密方案。
*/
//WiFi::EncrytionFlags WiFiScanResult::encrFlags() const
//{
//    Q_D(const WiFiScanResult);
//    return d->encrFlags;
//}

/*!
  设置 \a frequency 频率值，内部使用。
  */
//void WiFiScanResult::setEncrFlags(WiFi::EncrytionFlags encrs)
//{
//    Q_D(WiFiScanResult);
//    d->encrFlags = encrs;
//}

/*!
    返回描述访问点支持的身份验证、密钥管理和加密方案。
*/
QString WiFiScanResult::flags() const
{
    Q_D(const WiFiScanResult);
    return d->flags;
}

/*!
  设置 \a flags 描述访问点支持的身份验证、密钥管理和加密方案，内部使用。
  */
void WiFiScanResult::setFlags(const QString &flags)
{
    Q_D(WiFiScanResult);
    d->flags = flags;
}

/*!
    返回从启动开始到该扫描记录最后一次被发现经过的微秒数。
*/
qint64 WiFiScanResult::timestamp() const
{
    Q_D(const WiFiScanResult);
    return d->timestamp;
}

/*!
  设置 \a microseconds 从启动开始到该扫描记录最后一次被发现经过的微秒数，内部使用。
  */
void WiFiScanResult::setTimestamp(qint64 microseconds)
{
    Q_D(WiFiScanResult);
    d->timestamp = microseconds;
}

/*!
  每个配置的网络都有一个惟一的小整数ID，用于标识网络。
  */
int WiFiScanResult::networkId() const
{
    Q_D(const WiFiScanResult);
    return d->networkId;
}

/*!
  设置 \a id Network Id值，内部使用。
  */
void WiFiScanResult::setNetworkId(int id)
{
    Q_D(WiFiScanResult);
    d->networkId = id;
}


/*!
    如果频率值在2400~2500之间，（不包括2400和2500两个边界值），则返回true，否则返回false。
*/
bool WiFiScanResult::is24GHz() const
{
    Q_D(const WiFiScanResult);

    return d->frequency > 2400 && d->frequency < 2500;
}

/*!
    如果频率值在4900~5900之间，（不包括4900和5900两个边界值），则返回true，否则返回false。
*/
bool WiFiScanResult::is5GHz() const
{
    Q_D(const WiFiScanResult);

    return d->frequency > 4900 && d->frequency < 5900;
}

QString WiFiScanResult::toString() const
{
    Q_D(const WiFiScanResult);

    QString s(QStringLiteral("BSSID = %1\n"
                             "SSID  = %2\n"
                             "RSSI  = %3\n"
                             "Freq  = %4 MHz\n"
                             "Flags = %5\n"
                             "Times = %L6 ms\n"
                             "2.4G  = %7\n"
                             "5G    = %8\n"
                             "NetId = %9\n"));
    s = s.arg(d->bssid.toString());
    s = s.arg(d->ssid);
    s = s.arg(d->rssi);
    s = s.arg(d->frequency);
    s = s.arg(d->flags);
    s = s.arg(d->timestamp);
    s = s.arg(is24GHz() ? QLatin1String("true") : QLatin1String("false"));
    s = s.arg(is5GHz() ? QLatin1String("true") : QLatin1String("false"));
    s = s.arg(d->networkId);

    return s;
}

QVariantMap WiFiScanResult::toMap() const
{
    Q_D(const WiFiScanResult);
    QVariantMap map;

    map[QLatin1String("bssid")] = d->bssid.toString();
    map[QLatin1String("ssid")] = d->ssid;
    map[QLatin1String("rssi")] = d->rssi;
    map[QLatin1String("freq")] = d->frequency;
    map[QLatin1String("flags")] = d->flags;
    //    map[QLatin1String("auths")] = static_cast<int>(d->authFlags);
    //    map[QLatin1String("encrs")] = static_cast<int>(d->encrFlags);
    map[QLatin1String("timestamp")] = d->timestamp;
    map[QLatin1String("netId")] = d->networkId;

    return map;
}

QByteArray WiFiScanResult::toJson() const
{
    QJsonDocument doc = QJsonDocument::fromVariant(toMap());
    return doc.toJson(QJsonDocument::Compact);
}

WiFiScanResult WiFiScanResult::fromMap(const QVariantMap &map)
{
    WiFiMacAddress bssid(map[QLatin1String("bssid")].toString());
    if(bssid.isNull()) {
        qCritical() << "WiFiScanResult::fromMap. Error at: bssid is null." ;
        return WiFiScanResult();
    }

    QString ssid = map[QLatin1String("ssid")].toString();
    WiFiScanResult info(bssid, ssid);
    info.setRssi(map[QLatin1String("rssi")].toInt());
    info.setFrequency(map[QLatin1String("freq")].toInt());
    info.setFlags(map[QLatin1String("flags")].toString());
    //    int auths = map[QLatin1String("auths")].toInt();
    //    int encrs = map[QLatin1String("encrs")].toInt();
    //    info.setAuthFlags(static_cast<WiFi::AuthFlags>(auths));
    //    info.setEncrFlags(static_cast<WiFi::EncrytionFlags>(encrs));
    info.setTimestamp(map[QLatin1String("timestamp")].toLongLong());
    info.setNetworkId(map[QLatin1String("netId")].toInt());

    return info;
}

WiFiScanResult WiFiScanResult::fromJson(const QByteArray &json)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(json, &parseError);
    if (parseError.error) {
        qCritical() << "WiFiScanResult::fromJson. Error at:" << parseError.offset
                    << parseError.errorString();
        return WiFiScanResult();
    }
    return WiFiScanResult::fromMap(doc.toVariant().toMap());
}


QT_END_NAMESPACE
