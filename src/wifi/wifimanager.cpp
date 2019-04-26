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
#include "wifimanager_p.h"

#include "common/wpa_ctrl.h"

/*!
    \class WiFiManager
    \inmodule WiFi
    \brief 类 WiFiManager 提供用于管理 Wi-Fi 的 API 。
    \since 5.8

    该类提供用于管理 Wi-Fi 连接的所有方面的主要 API 。
*/

WiFiManagerPrivate::WiFiManagerPrivate()
    : QObjectPrivate()
    , tool(WiFiSupplicantTool::instance())
{
}

WiFiManagerPrivate::~WiFiManagerPrivate()
{
}

void WiFiManagerPrivate::_q_updateInfoTimeout()
{
    Q_Q(WiFiManager);

    WiFiInfo info = parser.fromStatus(tool->status());
    if(info != m_info) {
        m_info = info;

        Q_EMIT q->connectionInfoChanged();
    }

    WiFiNetworkList list = parser.fromListNetworks(tool->list_networks());
    int oldId = m_networks.isEmpty() ? -1 : m_networks.last().networkId();
    int newId = list.isEmpty() ? -1 : list.last().networkId();
    if(oldId == newId) {
        return;
    }

    m_networks.clear();
    m_netIdMapping.clear();

    for(int i = 0; i < list.length(); ++i) {
        WiFiNetwork network = list.at(i);
        int id = network.networkId();

        WiFiMacAddress bssid = WiFiMacAddress(tool->get_network(id,
                                              QStringLiteral("bssid")));
        QString proto = tool->get_network(id, QStringLiteral("proto"));
        QString key_mgmt = tool->get_network(id, QStringLiteral("key_mgmt"));
        QString pairwise = tool->get_network(id, QStringLiteral("pairwise"));
        QString psk = tool->get_network(id, QStringLiteral("psk"));
        network.setBSSID(bssid);
        network.setPreSharedKey(psk);
        network.setAuthFlags(parser.fromProtoKeyMgmt(proto, key_mgmt));
        network.setEncrFlags(parser.fromPairwise(pairwise));

        m_networks << network;
        m_netIdMapping.insert(bssid.toString() + network.ssid(), id);

    }
    Q_EMIT q->networksChanged();

    for(int i = 0; i < m_scanResults.length(); ++i) {
        QString key = m_scanResults[i].bssid().toString() + m_scanResults[i].ssid();
        int id = m_netIdMapping.value(key, -1);
        if(m_scanResults[i].networkId() != id) {
            m_scanResults[i].setNetworkId(id);
            Q_EMIT q->scanResultUpdated(m_scanResults[i]);
        }
    }
}

void WiFiManagerPrivate::onSupplicantStarted()
{
    Q_Q(WiFiManager);

    if(m_state == WiFi::StateEnabling || m_state == WiFi::StateEnabled) {
        tool->reassociate();
        m_state = WiFi::StateEnabled;
    } else {
        tool->disconnect();
        m_state = WiFi::StateDisabled;
    }

    if(!timer_Info) {
        timer_Info = new QTimer(q);
        timer_Info->setInterval(1000);
        timer_Info->connect(timer_Info, SIGNAL(timeout()), q,
                            SLOT(_q_updateInfoTimeout()));
    }
    timer_Info->start();
    Q_EMIT q->wifiStateChanged();
}

void WiFiManagerPrivate::onSupplicantFinished()
{
    Q_Q(WiFiManager);

    timer_Info->stop();

    if(m_state != WiFi::StateDisabled) {
        m_state = WiFi::StateDisabled;
        Q_EMIT q->wifiStateChanged();
    }
}

void WiFiManagerPrivate::onMessageReceived(const QString &msg)
{
    Q_Q(WiFiManager);

    if(msg.startsWith(QStringLiteral(WPA_EVENT_SCAN_RESULTS))) {

    } else if(msg.startsWith(QStringLiteral(WPA_EVENT_BSS_ADDED))) {
        QRegExp rx(QStringLiteral("(\\d+)(?:\\s*)"
                                  "([0-9a-fA-F]{2}(?:[:][0-9a-fA-F]{2}){5})"));
        int pos = rx.indexIn(msg);
        if (pos > -1) {
            // QString index = rxlen.cap(1);
            QString bssid = rx.cap(2);
            WiFiScanResult result = parser.fromBSS(tool->bss(bssid));
            if(result.isValid()) {
                QString key = result.bssid().toString() + result.ssid();
                result.setNetworkId(m_netIdMapping.value(key, -1));

                m_scanResults << result;
                Q_EMIT q->scanResultFound(result);
            }
        }
    } else if(msg.startsWith(QStringLiteral(WPA_EVENT_BSS_REMOVED))) {
        QRegExp rx(QStringLiteral("(\\d+)(?:\\s*)"
                                  "([0-9a-fA-F]{2}(?:[:][0-9a-fA-F]{2}){5})"));
        int pos = rx.indexIn(msg);
        if (pos > -1) {
            // QString index = rxlen.cap(1);
            QString bssid = rx.cap(2);
            int index = m_scanResults.indexOf(WiFiScanResult(bssid, QString()));
            if(index >= 0) {
                Q_EMIT q->scanResultLost(m_scanResults.takeAt(index));
            }
        }
    }
}

/*!
    构造一个 WiFiManager 对象。
*/
WiFiManager::WiFiManager(QObject * parent)
    : QObject(*(new WiFiManagerPrivate), parent)
{
    Q_D(WiFiManager);

    QObjectPrivate::connect(d->tool, &WiFiSupplicantTool::supplicantStarted, d,
                            &WiFiManagerPrivate::onSupplicantStarted);
    QObjectPrivate::connect(d->tool, &WiFiSupplicantTool::supplicantFinished, d,
                            &WiFiManagerPrivate::onSupplicantFinished);
    QObjectPrivate::connect(d->tool, &WiFiSupplicantTool::messageReceived, d,
                            &WiFiManagerPrivate::onMessageReceived);
}

WiFi::State WiFiManager::wifiState() const
{
    return WiFi::StateDisabling;
}

bool WiFiManager::isWiFiEnabled() const
{
    Q_D(const WiFiManager);
    return d->m_state != WiFi::StateEnabled;
}

void WiFiManager::setWifiEnabled(bool enabled)
{
    Q_D(WiFiManager);
    if(enabled) {
        if(d->m_state == WiFi::StateEnabling || d->m_state == WiFi::StateEnabled) {
            return;
        }

        if(!d->tool->isRunning()) {
            d->m_state = WiFi::StateDisabling;
            d->tool->start();
        } else {
            d->tool->reassociate();
            d->m_state = WiFi::StateEnabled;
        }
    } else {
        if(d->m_state == WiFi::StateDisabling || d->m_state == WiFi::StateDisabled) {
            return;
        }

        if(!d->tool->isRunning()) {
            d->m_state = WiFi::StateDisabled;
        } else {
            d->tool->disconnect();
            d->m_state = WiFi::StateDisabled;
        }
    }

    emit wifiStateChanged();
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
    Q_D(const WiFiManager);

    return d->m_info;
}

/*!
 * 返回最新访问点扫描的结果。
 */
WiFiScanResultList WiFiManager::scanResults() const
{
    Q_D(const WiFiManager);

    return d->m_scanResults;
}

/*!
 * 返回为当前前台用户配置的所有网络的列表。并不是所有 WiFiNetwork 字段都返回。
 */
WiFiNetworkList WiFiManager::networks() const
{
    Q_D(const WiFiManager);

    return d->m_networks;
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

#include "moc_wifimanager.cpp"
