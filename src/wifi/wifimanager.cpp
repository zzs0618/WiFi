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

#include "wifimanager.h"
#include "wifisupplicanttool_p.h"

#include <private/qobject_p.h>


/*!
    \class WiFiManager
    \inmodule WiFi
    \brief 类 WiFiManager 提供用于管理 Wi-Fi 的 API 。
    \since 5.8

    该类提供用于管理 Wi-Fi 连接的所有方面的主要 API 。
*/


class WiFiManagerPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(WiFiManager)
public:
    WiFiManagerPrivate();
    ~WiFiManagerPrivate();

    WiFiSupplicantTool *tool = NULL;

    bool m_enabled = false;
};

WiFiManagerPrivate::WiFiManagerPrivate()
    : QObjectPrivate()
    , tool(WiFiSupplicantTool::instance())
{
}

WiFiManagerPrivate::~WiFiManagerPrivate()
{
}

/*!
    构造一个 WiFiManager 对象。
*/
WiFiManager::WiFiManager(QObject *parent)
    : QObject(*(new WiFiManagerPrivate), parent)
{
}

WiFi::State WiFiManager::wifiState() const
{
    return WiFi::StateDisabling;
}

bool WiFiManager::isWiFiEnabled() const
{
    Q_D(const WiFiManager);
    return d->m_enabled;
}

void WiFiManager::setWifiEnabled(bool enabled)
{
    Q_D(WiFiManager);
    if(enabled != d->m_enabled) {
        d->m_enabled = enabled;
        emit wifiStateChanged();
    }
}

bool WiFiManager::is5GHzBandSupported() const
{
    return false;
}
bool WiFiManager::isP2pSupported() const
{
    return false;
}

/*!
 * 返回当前 Wi-Fi 连接的动态信息(如果有活动的话)。
 */
WiFiInfo WiFiManager::connectionInfo() const
{
    return WiFiInfo();
}

/*!
 * 返回最新访问点扫描的结果。
 */
WiFiScanResultList WiFiManager::scanResults() const
{
    return WiFiScanResultList();
}

/*!
 * 返回为当前前台用户配置的所有网络的列表。并不是所有 WiFiNetwork 字段都返回。
 */
WiFiNetworkList WiFiManager::networks() const
{
    return WiFiNetworkList();
}

/*!
 * \brief 计算信号的等级，这应该在显示信号时使用。
 * \param rssi 用RSSI测量信号的功率。
 * \param numLevels 在计算的级别中要考虑的等级范围。
 * \return 信号的等级，范围是[0, numLevels - 1]。
 */
quint16 WiFiManager::CalculateSignalLevel(int rssi, quint16 numLevels)
{
    if (rssi <= WiFi::MIN_RSSI) {
        return 0;
    } else if (rssi >= WiFi::MAX_RSSI) {
        return numLevels - 1;
    } else {
        int partitionSize = (WiFi::MAX_RSSI - WiFi::MIN_RSSI) / (numLevels - 1);
        return (rssi - WiFi::MIN_RSSI) / partitionSize;
    }
}

void WiFiManager::pingSupplicant()
{

}

void WiFiManager::saveConfiguration()
{

}

void WiFiManager::startScan()
{

}
