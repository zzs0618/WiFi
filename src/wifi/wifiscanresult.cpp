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
    QString capabilities;
    qint64 timestamp;
};

WiFiScanResultPrivate::WiFiScanResultPrivate() :
    valid(false),
    cached(false),
    rssi(-100),
    frequency(0),
    timestamp(0)
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
QString WiFiScanResult::capabilities() const
{
    Q_D(const WiFiScanResult);
    return d->capabilities;
}

/*!
  设置 \a flags 描述访问点支持的身份验证、密钥管理和加密方案，内部使用。
  */
void WiFiScanResult::setCapabilities(const QString &flags)
{
    Q_D(WiFiScanResult);
    d->capabilities = flags;
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


QT_END_NAMESPACE
