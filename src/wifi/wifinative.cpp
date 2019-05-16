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
Q_LOGGING_CATEGORY(logNat, "wifi.native", QtInfoMsg)

static int WIFI_NATIVE_NETWORK_CONNECT_TIMEOUT = 8; // seconds

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
    if(!qEnvironmentVariableIsEmpty("WIFI_NATIVE_NETWORK_CONNECT_TIMEOUT")) {
        bool ok;
        int timeout = qgetenv("WIFI_NATIVE_NETWORK_CONNECT_TIMEOUT").toInt(&ok);
        if(ok) {
            WIFI_NATIVE_NETWORK_CONNECT_TIMEOUT = timeout;
        }
    }
}

WiFiNativePrivate::~WiFiNativePrivate()
{
}

void WiFiNativePrivate::syncWiFiNetworks()
{
    Q_Q(WiFiNative);

    WiFiNetworkList list = parser.fromListNetworks(tool->list_networks());
    m_networks.clear();
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

    }
    WiFiNetwork network(m_info.networkId(), m_info.ssid());
    if(network.isValid() && !m_networks.contains(network)) {
        network.setBSSID(m_info.bssid());
        m_networks << network;
    }
    Q_EMIT q->networksChanged();
}

void WiFiNativePrivate::_q_updateInfoTimeout()
{
    Q_Q(WiFiNative);

    WiFiInfo info = parser.fromStatus(tool->status());
    bool ipChanged = m_info.ipAddress() != info.ipAddress();
    if(info != m_info) {
        m_info = info;
        qCDebug(logNat, "[ DEBUG ] ConnectionInfo:\n%s",
                qUtf8Printable(m_info.toString()));
        Q_EMIT q->connectionInfoChanged();
    }

    if(!m_info.ipAddress().isEmpty() && m_info.networkId() >= 0 && ipChanged) {
        qCInfo(logNat, "[ OK ] Network(%d, %s) connected with IP(%s).%s"
               , m_info.networkId(), qUtf8Printable(m_info.ssid())
               , qUtf8Printable(m_info.ipAddress())
               , wifiPrintTimes(timer_ConnNet->interval() - timer_ConnNet->remainingTime()));
        if(m_info.networkId() == timer_ConnNetId) {
            timer_ConnNetId = -1;
            timer_ConnNet->stop();
        }
        Q_EMIT q->networkConnected(m_info.networkId());
    }

    this->syncWiFiNetworks();

    for(int i = 0; i < m_scanResults.length(); ++i) {
        int id = getNetworkByScanResult(m_scanResults[i]).networkId();
        if(m_scanResults[i].networkId() != id) {
            qCDebug(logNat, "[ DEBUG ] Update ScanResult NetworkId (%s = %d) ",
                    qUtf8Printable(m_scanResults[i].ssid()), id);
            m_scanResults[i].setNetworkId(id);
            Q_EMIT q->scanResultUpdated(m_scanResults[i]);
        }
    }
}

void WiFiNativePrivate::_q_autoScanTimeout()
{
    tool->scan();
}

void WiFiNativePrivate::_q_connNetTimeout()
{
    Q_Q(WiFiNative);

    int networkId = timer_ConnNetId;
    const QString &ssid = getNetworkById(networkId).ssid();
    qCWarning(logNat, "[FAIL] Network(%d, %s) authenticate timeout.%s"
              , networkId, qUtf8Printable(ssid), wifiPrintTimes(timer_ConnNet->interval()));
    timer_ConnNetId = -1;
    tool->remove_network(networkId);
    Q_EMIT q->networkErrorOccurred(networkId);
}

void WiFiNativePrivate::onSupplicantStarted()
{
    Q_Q(WiFiNative);

    if(m_state != WiFi::StateEnabling) {
        return;
    }

    tool->reassociate();
    m_state = WiFi::StateEnabled;

    if(!timer_Scan) {
        timer_Scan = new QTimer(q);
        timer_Scan->setSingleShot(true);
        timer_Scan->setInterval(1000);
        timer_Scan->connect(timer_Scan, SIGNAL(timeout()), q,
                            SLOT(_q_autoScanTimeout()));
    }

    if(!timer_ConnNet) {
        timer_ConnNet = new QTimer(q);
        timer_ConnNet->setSingleShot(true);
        timer_ConnNet->setInterval(WIFI_NATIVE_NETWORK_CONNECT_TIMEOUT * 1000);
        timer_ConnNet->connect(timer_ConnNet, SIGNAL(timeout()), q,
                               SLOT(_q_connNetTimeout()));
    }

    if(!timer_Info) {
        timer_Info = new QTimer(q);
        timer_Info->setInterval(1000);
        timer_Info->connect(timer_Info, SIGNAL(timeout()), q,
                            SLOT(_q_updateInfoTimeout()));
    }

    this->initWiFiNativeInfo();

    timer_Info->start();

    if(m_isAutoScan) {
        tool->scan();
    }

    Q_EMIT q->wifiStateChanged();
}

void WiFiNativePrivate::initWiFiNativeInfo()
{
    Q_Q(WiFiNative);

    WiFiInfo info = parser.fromStatus(tool->status());
    m_info = info;
    if(!m_info.ipAddress().isEmpty() && m_info.networkId() < 0) {
        tool->dhcpc_release();
    }
    Q_EMIT q->connectionInfoChanged();

    this->syncWiFiNetworks();

    QStringList bssids = parser.fromScanResult(tool->scan_results());
    for(const QString &bssid : bssids) {
        WiFiScanResult result = parser.fromBSS(tool->bss(bssid));
        if(result.isValid()) {
            int id = getNetworkByScanResult(result).networkId();
            result.setNetworkId(id);
            m_scanResults << result;
            Q_EMIT q->scanResultFound(result);
        }
    }
}

void WiFiNativePrivate::onSupplicantFinished()
{
    Q_Q(WiFiNative);

    tool->disconnect();
    m_state = WiFi::StateDisabled;

    if(timer_Scan) {
        timer_Scan->stop();
    }
    if(timer_ConnNet) {
        timer_ConnNet->stop();
    }
    if(timer_Info) {
        timer_Info->stop();
    }

    m_isAutoScan = false;
    Q_EMIT q->isAutoScanChanged();

    m_info = WiFiInfo();
    Q_EMIT q->connectionInfoChanged();

    m_networks.clear();
    Q_EMIT q->networksChanged();

    for(const WiFiScanResult &sr : m_scanResults) {
        Q_EMIT q->scanResultLost(sr);
    }
    m_scanResults.clear();

    Q_EMIT q->wifiStateChanged();
}

void WiFiNativePrivate::onMessageReceived(const QString &msg)
{
    Q_Q(WiFiNative);

    if(msg.startsWith(QStringLiteral(WPA_EVENT_SCAN_RESULTS))) {
        if(timer_ConnNet->isActive()) {
            tool->scan();
        }else if(m_isAutoScan) {
            timer_Scan->start();
        }
    } else if(q->isWiFiEnabled() && msg.startsWith(QStringLiteral(WPA_EVENT_BSS_ADDED))) {
        QRegExp rx(QStringLiteral("(\\d+)(?:\\s*)"
                                  "([0-9a-fA-F]{2}(?:[:][0-9a-fA-F]{2}){5})"));
        int pos = rx.indexIn(msg);
        if (pos > -1) {
            // QString index = rxlen.cap(1);
            QString bssid = rx.cap(2);
            WiFiScanResult result = parser.fromBSS(tool->bss(bssid));
            if(result.isValid()) {
                int id = getNetworkByScanResult(result).networkId();
                result.setNetworkId(id);

                m_scanResults << result;
                Q_EMIT q->scanResultFound(result);
            }
        }
    } else if(q->isWiFiEnabled() && msg.startsWith(QStringLiteral(WPA_EVENT_BSS_REMOVED))) {
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
    } else if(msg.startsWith(QStringLiteral(WPA_EVENT_TEMP_DISABLED))) {
        // CTRL-EVENT-SSID-TEMP-DISABLED id=1 ssid=\"hsaeyz\" auth_failures=1 duration=10 reason=WRONG_KEY
        QString ssid;
        int networkId = -1;
        QStringList items = msg.split(QRegExp(QStringLiteral(" ")));
        for (int i = 0; i < items.size(); i++) {
            QString str = items.at(i);
            if (str.startsWith(QStringLiteral("id="))) {
                bool ok;
                int id = str.section(QLatin1Char('='), 1).trimmed().toInt(&ok);
                if(ok) {
                    networkId = id;
                }
            } else if(str.startsWith(QStringLiteral("ssid="))) {
                ssid = str.section(QLatin1Char('"'), 1);
            }
        }
        if(networkId == timer_ConnNetId) {
            qCWarning(logNat,
                      "[FAIL] Network(%d, %s) authenticate failed.%s\n%s"
                      , networkId, qUtf8Printable(ssid)
                      , wifiPrintTimes(timer_ConnNet->interval() - timer_ConnNet->remainingTime())
                      , qUtf8Printable(msg));
            timer_ConnNetId = -1;
            timer_ConnNet->stop();
            tool->remove_network(networkId);
            Q_EMIT q->networkErrorOccurred(networkId);
        }
    } else if(msg.startsWith(QStringLiteral(WPA_EVENT_CONNECTED))) {
        // CTRL-EVENT-CONNECTED - Connection to 0c:4b:54:7a:21:21 completed [id=2 id_str=]
        int networkId = -1;
        QStringList items = msg.split(QRegExp(QStringLiteral(" ")));
        for (int i = 0; i < items.size(); i++) {
            QString str = items.at(i);
            if (str.startsWith(QStringLiteral("[id="))) {
                bool ok;
                int id = str.section(QLatin1Char('='), 1).trimmed().toInt(&ok);
                if(ok) {
                    networkId = id;
                }
            }
        }

        const QString &ssid = getNetworkById(networkId).ssid();
        if(timer_ConnNet->isActive()) {
            qCInfo(logNat, "[ OK ] Network(%d, %s) authenticated.%s"
                   , networkId, qUtf8Printable(ssid)
                   , wifiPrintTimes(timer_ConnNet->interval() - timer_ConnNet->remainingTime()));
        } else {
            qCInfo(logNat, "[ OK ] Network(%d, %s) authenticated.[ auto ]"
                   , networkId, qUtf8Printable(ssid));
        }
        Q_EMIT q->networkAuthenticated(networkId);

        if(m_info.ipAddress().isEmpty()) {
            tool->dhcpc_request();
        }
        this->_q_updateInfoTimeout();
    } else if(msg.startsWith(QStringLiteral(WPA_EVENT_DISCONNECTED))) {
        int networkId = m_info.networkId();
        const QString &ssid = getNetworkById(networkId).ssid();
        qCInfo(logNat, "[ OK ] Network(%d, %s) disconnected.", networkId, qUtf8Printable(ssid));
        if(!m_info.ipAddress().isEmpty()) {
            tool->dhcpc_release();
        }
        this->_q_updateInfoTimeout();
    }
}

bool WiFiNativePrivate::compare(const WiFiScanResult &scanResult, const WiFiNetwork &network) const
{
    if(!network.bssid().isNull() && network.bssid() == scanResult.bssid()) {
        return true;
    }else if(network.ssid() == scanResult.ssid()) {
        return true;
    }
    return false;
}

WiFiNetwork WiFiNativePrivate::getNetworkById(int id) const
{
    for(const WiFiNetwork &net : m_networks) {
        if(net.networkId() == id) {
            return net;
        }
    }
    return WiFiNetwork();
}

WiFiScanResult WiFiNativePrivate::getScanResultByNetwork(const WiFiNetwork &network) const
{
    for(const WiFiScanResult &scanResult : m_scanResults) {
        if(compare(scanResult, network)) {
            return scanResult;
        }
    }
    return WiFiScanResult();
}

WiFiNetwork WiFiNativePrivate::getNetworkByScanResult(const WiFiScanResult &scanResult) const
{
    for(const WiFiNetwork &network : m_networks) {
        if(compare(scanResult, network)) {
            return network;
        }
    }
    return WiFiNetwork();
}

int WiFiNativePrivate::addNetwork(const WiFiNetwork &network)
{
    bool ok;
    int id = tool->add_network().toInt(&ok);
    if(ok) {
        WiFiNetwork newNet = WiFiNetwork(id, network.ssid());
        newNet.setBSSID(network.bssid());
        newNet.setAuthFlags(network.authFlags());
        newNet.setEncrFlags(network.encrFlags());
        newNet.setPreSharedKey(network.preSharedKey());
        return this->editNetwork(newNet);
    }
    return id;
}

int WiFiNativePrivate::editNetwork(const WiFiNetwork &network)
{
    int id = network.networkId();
    if(id < 0) {
        return id;
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
            pairwise = QStringLiteral("TKIP CCMP");
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
        qCCritical(logNat, "[FAIL] Network(%d, %s) enable failed.\n%s"
                   , id, qUtf8Printable(network.ssid()), qUtf8Printable(result));
    }

    this->selectNetwork(id);
    tool->save_config();

    return id;
}

void WiFiNativePrivate::selectNetwork(int networkId)
{
    Q_Q(WiFiNative);
    Q_EMIT q->networkConnecting(networkId);
    timer_ConnNetId = networkId;
    timer_ConnNet->start();
    tool->select_network(networkId);
}

void WiFiNativePrivate::removeNetwork(int networkId)
{
    tool->remove_network(networkId);
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
    wifiTrace(logNat);
    Q_D(WiFiNative);
    if(enabled) {
        if(d->m_state == WiFi::StateEnabling || d->m_state == WiFi::StateEnabled) {
            return;
        }

        d->m_state = WiFi::StateEnabling;
        emit wifiStateChanged();

        if(!d->tool->isRunning()) {
            d->tool->start();
        } else {
            d->onSupplicantStarted();
        }
    } else {
        if(d->m_state == WiFi::StateDisabling || d->m_state == WiFi::StateDisabled) {
            return;
        }

        d->m_state = WiFi::StateDisabling;
        emit wifiStateChanged();

        if(d->tool->isRunning()) {
            d->onSupplicantFinished();
        }
    }
}

bool WiFiNative::isAutoScan() const
{
    Q_D(const WiFiNative);
    return d->m_isAutoScan;
}

void WiFiNative::setAutoScan(bool enabled)
{
    wifiTrace(logNat);
    Q_D(WiFiNative);
    if(d->m_isAutoScan == enabled) {
        return;
    }
    d->m_isAutoScan = enabled;
    emit isAutoScanChanged();

    if(d->m_isAutoScan && isWiFiEnabled()) {
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

int WiFiNative::addNetwork(const WiFiNetwork &network)
{
    wifiTrace(logNat);
    Q_D(WiFiNative);
    if(network.isValid()) {
        return d->editNetwork(network);
    } else {
        return d->addNetwork(network);
    }
}

void WiFiNative::selectNetwork(int networkId)
{
    wifiTrace(logNat);
    Q_D(WiFiNative);
    d->selectNetwork(networkId);
}

void WiFiNative::removeNetwork(int networkId)
{
    wifiTrace(logNat);
    Q_D(WiFiNative);
    d->removeNetwork(networkId);
}

#include "moc_wifinative.cpp"
