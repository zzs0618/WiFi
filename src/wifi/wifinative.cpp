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

#include "wifinative.h"
#include "wifinative_p.h"

#include "common/wpa_ctrl.h"

// in a header
Q_DECLARE_LOGGING_CATEGORY(logNat)
// in one source file
Q_LOGGING_CATEGORY(logNat, "wifi.native")

/*!
    \class WiFiNative
    \inmodule WiFi
    \brief 类 WiFiNative 提供用于集成 Wi-Fi 本地的 API 。
    \since 5.8

    该类提供用于集成 Wi-Fi 功能的所有方面的主要 API 。
*/

WiFiNativePrivate::WiFiNativePrivate()
    : QObjectPrivate()
    , tool(WiFiSupplicantTool::instance())
{
}

WiFiNativePrivate::~WiFiNativePrivate()
{
}

void WiFiNativePrivate::_q_updateInfoTimeout()
{
    Q_Q(WiFiNative);

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

void WiFiNativePrivate::_q_autoScanTimeout()
{
    tool->scan();
}

void WiFiNativePrivate::onSupplicantStarted()
{
    Q_Q(WiFiNative);

    if(m_state == WiFi::StateEnabling || m_state == WiFi::StateEnabled) {
        tool->reassociate();
        m_state = WiFi::StateEnabled;
    } else {
        tool->disconnect();
        m_state = WiFi::StateDisabled;
    }

    if(!timer_Scan) {
        timer_Scan = new QTimer(q);
        timer_Scan->setSingleShot(true);
        timer_Scan->setInterval(1000);
        timer_Scan->connect(timer_Scan, SIGNAL(timeout()), q,
                            SLOT(_q_autoScanTimeout()));
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

void WiFiNativePrivate::onSupplicantFinished()
{
    Q_Q(WiFiNative);

    timer_Info->stop();

    if(m_state != WiFi::StateDisabled) {
        m_state = WiFi::StateDisabled;
        Q_EMIT q->wifiStateChanged();
    }
}

void WiFiNativePrivate::onMessageReceived(const QString &msg)
{
    Q_Q(WiFiNative);

    if(msg.startsWith(QStringLiteral(WPA_EVENT_SCAN_RESULTS))) {
        if(m_isAutoScan) {
            timer_Scan->start();
        }
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

void WiFiNativePrivate::addNetwork(const WiFiNetwork &network)
{
    bool ok;
    int id = tool->add_network().toInt(&ok);
    if(ok) {
        WiFiNetwork newNet = WiFiNetwork(id, network.ssid());
        newNet.setBSSID(network.bssid());
        newNet.setAuthFlags(network.authFlags());
        newNet.setEncrFlags(network.encrFlags());
        newNet.setPreSharedKey(network.preSharedKey());
        this->editNetwork(newNet);
    }
}

void WiFiNativePrivate::editNetwork(const WiFiNetwork &network)
{
    int id = network.networkId();
    if(id < 0) {
        return;
    }

    tool->set_network(id, QLatin1String("ssid"), network.ssid());

    WiFi::AuthFlags auth = network.authFlags();
    if(auth.testFlag(WiFi::NoneWEPShared)) {
        tool->set_network(id, QLatin1String("auth_alg"), QStringLiteral("SHARED"));
    } else {
        tool->set_network(id, QLatin1String("auth_alg"), QStringLiteral("OPEN"));
    }


    QStringList key_mgmt;
    QString proto, pairwise;
    if(auth == WiFi::NoneOpen || auth.testFlag(WiFi::NoneWEP) ||
       auth.testFlag(WiFi::NoneWEPShared)) {
        key_mgmt << QStringLiteral("NONE");
    }
    if(auth.testFlag(WiFi::IEEE8021X)) {
        key_mgmt << QStringLiteral("IEEE8021X");
    }
    if(auth.testFlag(WiFi::WPA2_PSK)) {
        key_mgmt << QStringLiteral("WPA-PSK");
        proto = QStringLiteral("WPA2");
    } else if(auth.testFlag(WiFi::WPA_PSK)) {
        key_mgmt << QStringLiteral("WPA-PSK");
        proto = QStringLiteral("WPA");
    }
    if(auth.testFlag(WiFi::WPA2_EAP)) {
        key_mgmt << QStringLiteral("WPA-EAP");
        proto = QStringLiteral("WPA2");
    } else if(auth.testFlag(WiFi::WPA_EAP)) {
        key_mgmt << QStringLiteral("WPA-EAP");
        proto = QStringLiteral("WPA");
    }
    if(!key_mgmt.isEmpty()) {
        tool->set_network(id, QLatin1String("key_mgmt"),
                          key_mgmt.join(QLatin1Char(' ')));
    }
    if(!proto.isEmpty()) {
        tool->set_network(id, QLatin1String("proto"), proto);
    }
    if (auth.testFlag(WiFi::WPA_PSK) || auth.testFlag(WiFi::WPA_EAP) ||
        auth.testFlag(WiFi::WPA2_PSK) || auth.testFlag(WiFi::WPA2_EAP)) {
        int encr = (auth.testFlag(WiFi::WPA_PSK) ||
                    auth.testFlag(WiFi::WPA_EAP)) ? 0 : 1;
        if (encr == 0) {
            pairwise = QStringLiteral("TKIP");
        } else {
            pairwise = QStringLiteral("CCMP");
        }
    }
    if(!pairwise.isEmpty()) {
        tool->set_network(id, QLatin1String("pairwise"), pairwise);
        tool->set_network(id, QLatin1String("group"),
                          QStringLiteral("TKIP CCMP WEP104 WEP40"));
    }


    if(auth.testFlag(WiFi::WPA_PSK) || auth.testFlag(WiFi::WPA2_PSK)) {
        tool->set_network(id, QStringLiteral("psk"), network.preSharedKey());
    }

    QString result = tool->enable_network(id);
    if(!result.startsWith(QStringLiteral("OK"))) {
        qCCritical(logWiFi,
                   "Failed to enable network in wpa_supplicant configuration.\n%s",
                   qUtf8Printable(result));
    }

    tool->save_config();
}

/*!
    构造一个 WiFiNative 对象。
*/
WiFiNative::WiFiNative(QObject * parent)
    : QObject(*(new WiFiNativePrivate), parent)
{
    Q_D(WiFiNative);

    QObjectPrivate::connect(d->tool, &WiFiSupplicantTool::supplicantStarted, d,
                            &WiFiNativePrivate::onSupplicantStarted);
    QObjectPrivate::connect(d->tool, &WiFiSupplicantTool::supplicantFinished, d,
                            &WiFiNativePrivate::onSupplicantFinished);
    QObjectPrivate::connect(d->tool, &WiFiSupplicantTool::messageReceived, d,
                            &WiFiNativePrivate::onMessageReceived);
}

WiFi::State WiFiNative::wifiState() const
{
    Q_D(const WiFiNative);
    return d->m_state;
}

bool WiFiNative::isWiFiEnabled() const
{
    Q_D(const WiFiNative);
    return d->m_state == WiFi::StateEnabled;
}

void WiFiNative::setWiFiEnabled(bool enabled)
{
    Q_D(WiFiNative);
    if(enabled) {
        if(d->m_state == WiFi::StateEnabling || d->m_state == WiFi::StateEnabled) {
            return;
        }

        if(!d->tool->isRunning()) {
            d->m_state = WiFi::StateEnabling;
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

bool WiFiNative::isAutoScan() const
{
    Q_D(const WiFiNative);
    return d->m_isAutoScan;
}

void WiFiNative::setAutoScan(bool enabled)
{
    Q_D(WiFiNative);
    if(d->m_isAutoScan == enabled) {
        return;
    }
    d->m_isAutoScan = enabled;
    emit isAutoScanChanged();

    if(d->m_isAutoScan) {
        startScan();
    }
}

bool WiFiNative::is5GHzBandSupported() const
{
    return false;
}
bool WiFiNative::isP2pSupported() const
{
    return false;
}

/*!
 * 返回当前 Wi-Fi 连接的动态信息(如果有活动的话)。
 */
WiFiInfo WiFiNative::connectionInfo() const
{
    Q_D(const WiFiNative);

    return d->m_info;
}

/*!
 * 返回最新访问点扫描的结果。
 */
WiFiScanResultList WiFiNative::scanResults() const
{
    Q_D(const WiFiNative);

    return d->m_scanResults;
}

/*!
 * 返回为当前前台用户配置的所有网络的列表。并不是所有 WiFiNetwork 字段都返回。
 */
WiFiNetworkList WiFiNative::networks() const
{
    Q_D(const WiFiNative);

    return d->m_networks;
}

/*!
 * \brief 计算信号的等级，这应该在显示信号时使用。
 * \param rssi 用RSSI测量信号的功率。
 * \param numLevels 在计算的级别中要考虑的等级范围。
 * \return 信号的等级，范围是[0, numLevels - 1]。
 */
quint16 WiFiNative::CalculateSignalLevel(int rssi, quint16 numLevels)
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

void WiFiNative::pingSupplicant()
{
    Q_D(WiFiNative);
    d->tool->ping();
}

void WiFiNative::saveConfiguration()
{
    Q_D(WiFiNative);
    d->tool->save_config();
}

void WiFiNative::startScan()
{
    QTimer::singleShot(0, this, SLOT(_q_autoScanTimeout()));
}

void WiFiNative::addNetwork(const WiFiNetwork &network)
{
    Q_D(WiFiNative);
    if(network.isValid()) {
        d->editNetwork(network);
    } else {
        d->addNetwork(network);
    }
}

#include "moc_wifinative.cpp"
