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

#include "wifinetwork.h"
#include "wifimacaddress.h"

#include <QtCore/qjsondocument.h>


QT_BEGIN_NAMESPACE

class WiFiNetworkPrivate
{
public:
    WiFiNetworkPrivate();

    bool valid;
    bool cached;

    int networkId;
    QString ssid;
    WiFiMacAddress bssid;
    WiFi::AuthFlags authFlags;
    WiFi::EncrytionFlags encrFlags;
    QString preSharedKey;
};

WiFiNetworkPrivate::WiFiNetworkPrivate() :
    valid(false),
    cached(false),
    networkId(-1),
    authFlags(WiFi::NoneOpen),
    encrFlags(WiFi::None)
{
}

/*!
    \class WiFiNetwork
    \inmodule WiFi
    \brief 类 WiFiNetwork 保存了获取的网络连接的相关信息。
    \since 5.8

    描述有关获取到的网络连接的信息。
*/


/*!
    构造一个无效的 WiFiNetwork 对象。
*/
WiFiNetwork::WiFiNetwork() :
    d_ptr(new WiFiNetworkPrivate)
{

}

/*!
    构造一个 WiFiNetwork 对象，该对象具有 SSID 。
*/
WiFiNetwork::WiFiNetwork(const QString &ssid) :
    d_ptr(new WiFiNetworkPrivate)
{
    Q_D(WiFiNetwork);

    d->ssid = ssid;
    d->valid = false;
    d->cached = false;
}

/*!
    构造一个 WiFiNetwork 对象，该对象具有 Network Id 和 SSID 。
*/
WiFiNetwork::WiFiNetwork(int id, const QString &ssid) :
    d_ptr(new WiFiNetworkPrivate)
{
    Q_D(WiFiNetwork);

    d->networkId = id;
    d->ssid = ssid;
    if(d->networkId >= 0) {
        d->valid = true;
    }
    d->cached = false;
}

/*!
    构造一个 WiFiNetwork 对象，它是 \a other 的副本。
*/
WiFiNetwork::WiFiNetwork(const WiFiNetwork &other) :
    d_ptr(new WiFiNetworkPrivate)
{
    *this = other;
}

/*!
    销毁 WiFiNetwork 对象。
*/
WiFiNetwork::~WiFiNetwork()
{
    delete d_ptr;
}

/*!
    如果 WiFiNetwork 对象有效，则返回 true， 否则返回 false 。
*/
bool WiFiNetwork::isValid() const
{
    Q_D(const WiFiNetwork);
    return d->valid;
}

/*!
     如果从缓存的数据创建 WiFiNetwork 对象，则返回true。
 */
bool WiFiNetwork::isCached() const
{
    Q_D(const WiFiNetwork);
    return d->cached;
}

/*!
    如果从缓存的数据创建 WiFiNetwork 对象，则系统将使用它设置 \a cached 缓存标志。
    缓存信息可能不如从活动设备读取的数据准确。
  */
void WiFiNetwork::setCached(bool cached)
{
    Q_D(WiFiNetwork);
    d->cached = cached;
}

/*!
    将\a other分配到此 WiFiNetwork 对象。
*/
WiFiNetwork &WiFiNetwork::operator=(const WiFiNetwork &other)
{
    Q_D(WiFiNetwork);

    d->networkId = other.d_func()->networkId;
    d->ssid = other.d_func()->ssid;
    d->valid = other.d_func()->valid;
    d->cached = other.d_func()->cached;
    d->authFlags = other.d_func()->authFlags;
    d->encrFlags = other.d_func()->encrFlags;

    return *this;
}

/*!
    将此 WiFiNetwork 与 \a other 地址进行比较。

    如果两个 WiFiNetwork 相等，返回 true ，否则返回 false 。
  */
bool WiFiNetwork::operator==(const WiFiNetwork &other) const
{
    Q_D(const WiFiNetwork);

    return d->ssid == other.d_func()->ssid;
}

/*!
    如果此对象与 \a other 比较是否不同，不同则返回true，否则返回false。

    \sa operator==()
*/
bool WiFiNetwork::operator!=(const WiFiNetwork &other) const
{
    return !(*this == other);
}

/*!
  每个配置的网络都有一个惟一的小整数ID，用于标识网络。
  */
int WiFiNetwork::networkId() const
{
    Q_D(const WiFiNetwork);
    return d->networkId;
}

/*!
    返回网络连接的网络名称。
*/
QString WiFiNetwork::ssid() const
{
    Q_D(const WiFiNetwork);
    return d->ssid;
}

/*!
    返回网络连接的地址。
*/
WiFiMacAddress WiFiNetwork::bssid() const
{
    Q_D(const WiFiNetwork);
    return d->bssid;
}

/*!
  设置 \a bssid 认证方式，内部使用。
  */
void WiFiNetwork::setBSSID(const WiFiMacAddress &bssid)
{
    Q_D(WiFiNetwork);
    d->bssid = bssid;
}

/*!
    返回描述访问点支持的身份验证、密钥管理和加密方案。
*/
WiFi::AuthFlags WiFiNetwork::authFlags() const
{
    Q_D(const WiFiNetwork);
    return d->authFlags;
}

/*!
  设置 \a auths 认证方式，内部使用。
  */
void WiFiNetwork::setAuthFlags(WiFi::AuthFlags auths)
{
    Q_D(WiFiNetwork);
    d->authFlags = auths;
}

/*!
    返回描述访问点支持的身份验证、密钥管理和加密方案。
*/
WiFi::EncrytionFlags WiFiNetwork::encrFlags() const
{
    Q_D(const WiFiNetwork);
    return d->encrFlags;
}

/*!
  设置 \a encrs 加密方式，内部使用。
  */
void WiFiNetwork::setEncrFlags(WiFi::EncrytionFlags encrs)
{
    Q_D(WiFiNetwork);
    d->encrFlags = encrs;
}

/*!
    返回与WPA-PSK一起使用的预共享密钥。
*/
QString WiFiNetwork::preSharedKey() const
{
    Q_D(const WiFiNetwork);
    return d->preSharedKey;
}

/*!
  设置 \a psk 预共享密钥，内部使用。
  */
void WiFiNetwork::setPreSharedKey(const QString &psk)
{
    Q_D(WiFiNetwork);
    d->preSharedKey = psk;
}


QString WiFiNetwork::toString() const
{
    Q_D(const WiFiNetwork);

    QString s(QStringLiteral("NetId = %1\n"
                             "SSID  = %2\n"
                             "BSSID = %3\n"
                             "Auths = %4\n"
                             "Encrs = %5\n"
                             "NetId = %6\n"));
    s = s.arg(d->networkId);
    s = s.arg(d->ssid);
    s = s.arg(d->bssid.toString());
    s = s.arg(WiFi::toString(d->authFlags));
    s = s.arg(WiFi::toString(d->encrFlags));
    s = s.arg(d->preSharedKey);

    return s;
}

QVariantMap WiFiNetwork::toMap() const
{
    Q_D(const WiFiNetwork);
    QVariantMap map;

    map[QLatin1String("networkId")] = d->networkId;
    map[QLatin1String("ssid")] = d->ssid;
    map[QLatin1String("bssid")] = d->bssid.toString();
    map[QLatin1String("authFlags")] = static_cast<int>(d->authFlags);
    map[QLatin1String("encrFlags")] = static_cast<int>(d->encrFlags);
    map[QLatin1String("preSharedKey")] = d->preSharedKey;

    return map;
}

QByteArray WiFiNetwork::toJson() const
{
    QJsonDocument doc = QJsonDocument::fromVariant(toMap());
    return doc.toJson(QJsonDocument::Compact);
}

WiFiNetwork WiFiNetwork::fromMap(const QVariantMap &map)
{
    int id = map[QLatin1String("networkId")].toInt();
    QString ssid = map[QLatin1String("ssid")].toString();
    WiFiNetwork info(id, ssid);
    QString bssid = map[QLatin1String("bssid")].toString();
    info.setBSSID(WiFiMacAddress(bssid));

    int auths = map[QLatin1String("authFlags")].toInt();
    int encrs = map[QLatin1String("encrFlags")].toInt();
    info.setAuthFlags(static_cast<WiFi::AuthFlags>(auths));
    info.setEncrFlags(static_cast<WiFi::EncrytionFlags>(encrs));
    info.setPreSharedKey(map[QLatin1String("preSharedKey")].toString());

    return info;
}

WiFiNetwork WiFiNetwork::fromJson(const QByteArray &json)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(json, &parseError);
    if (parseError.error) {
        qCritical() << "WiFiNetwork::fromJson. Error at:" << parseError.offset
                    << parseError.errorString();
        return WiFiNetwork();
    }
    return WiFiNetwork::fromMap(doc.toVariant().toMap());
}

QVariantList WiFiNetworkList::toMapList() const
{
    QVariantList maps;
    for(int i = 0; i < size(); ++i) {
        maps << at(i).toMap();
    }
    return maps;
}
QByteArray WiFiNetworkList::toJson() const
{
    QJsonDocument doc = QJsonDocument::fromVariant(toMapList());
    return doc.toJson(QJsonDocument::Compact);
}

WiFiNetworkList WiFiNetworkList::fromMapList(const QVariantList
        &mapList)
{
    WiFiNetworkList list;
    for(int i = 0; i < mapList.size(); ++i) {
        list << WiFiNetwork::fromMap(mapList.at(i).toMap());
    }
    return list;
}

WiFiNetworkList WiFiNetworkList::fromJson(const QByteArray &json)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(json, &parseError);
    if (parseError.error) {
        qCritical() << "WiFiScanResultList::fromJson. Error at:" << parseError.offset
                    << parseError.errorString();
        return WiFiNetworkList();
    }
    return WiFiNetworkList::fromMapList(doc.toVariant().toList());
}


QT_END_NAMESPACE
