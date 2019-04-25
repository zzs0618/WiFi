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

#ifndef WIFISUPPLICANTTOOL_P_H
#define WIFISUPPLICANTTOOL_P_H

#include <QtCore/qobject.h>
#include <QtCore/qloggingcategory.h>

#include <private/qobject_p.h>
#include <QtCore/qtimer.h>
#include <QtCore/qprocess.h>
#include <QtCore/qsocketnotifier.h>

// in a header
Q_DECLARE_LOGGING_CATEGORY(logWPA)

QT_BEGIN_NAMESPACE

class WiFiSupplicantToolPrivate;
class WiFiSupplicantTool : public QObject
{
    Q_OBJECT
public:
    static WiFiSupplicantTool *instance();

    /* PING: 此命令可用于测试 wpa_supplicant 是否响应控制接口命令。
     * 如果连接打开且 wpa_supplicant 正在处理命令，则预期的响应是: PONG 。
     */
    QString ping() const;

    /* STATUS: 请求当前WPA/EAPOL/EAP状态信息。输出是一个文本块，每一行的格式都是variable=value。
     * 例如(verbose=false):
     *    bssid=02:00:01:02:03:04
     *    ssid=test network
     *    pairwise_cipher=CCMP
     *    group_cipher=CCMP
     *    key_mgmt=WPA-PSK
     *    wpa_state=COMPLETED
     *    ip_address=192.168.1.21
     *    Supplicant PAE state=AUTHENTICATED
     *    suppPortStatus=Authorized
     *    EAP state=SUCCESS
     * 例如(verbose=true):
     *    bssid=02:00:01:02:03:04
     *    ssid=test network
     *    id=0
     *    pairwise_cipher=CCMP
     *    group_cipher=CCMP
     *    key_mgmt=WPA-PSK
     *    wpa_state=COMPLETED
     *    ip_address=192.168.1.21
     *    Supplicant PAE state=AUTHENTICATED
     *    suppPortStatus=Authorized
     *    heldPeriod=60
     *    authPeriod=30
     *    startPeriod=30
     *    maxStart=3
     *    portControl=Auto
     *    Supplicant Backend state=IDLE
     *    EAP state=SUCCESS
     *    reqMethod=0
     *    methodState=NONE
     *    decision=COND_SUCC
     *    ClientTimeout=60
     */
    QString status(bool verbose = false) const;

    /* REASSOCIATE: 强制重新关联。
     */
    QString reassociate() const;

    /* RECONNECT: 如果断开则连接(类似 REASSOCIATE ，但只有在断开连接的状态下才连接)。
     */
    QString reconnect() const;

    /* DISCONNECT: 断开连接，等待 REASSOCIATE 或 RECONNECT 命令后再连接。
     */
    QString disconnect() const;

    /* RECONFIGURE: 强制 wpa_supplicant 重新读取其配置数据。
     */
    QString reconfigure() const;

    /* TERMINATE: 终止 wpa_supplicant 进程。
     */
    QString terminate() const;

    /* LIST_NETWORKS: 获取配置网络列表。
     * 例如：
     * network id / ssid / bssid / flags
     * 0       example network any     [CURRENT]
     */
    QString list_networks() const;

    /* SCAN: 请求一个新的BSS扫描。
     */
    QString scan() const;

    /* SCAN_RESULTS: 获取最新的扫描结果。
     * 例如：
     * bssid / frequency / signal level / flags / ssid
     * 00:09:5b:95:e0:4e       2412    -40     [WPA-PSK-CCMP]  jkm private
     * 02:55:24:33:77:a3       2462    -69     [WPA-PSK-TKIP]  testing
     * 00:09:5b:95:e0:4f       2412    -78             jkm guest
     */
    QString scan_results() const;

    /* BSS: 命令 "BSS <idx>" 获得详细的每个 BSS 扫描结果。
     * 例如：
     * bssid=00:09:5b:95:e0:4e
     * freq=2412
     * beacon_int=0
     * capabilities=0x0011
     * qual=51
     * noise=161
     * level=212
     * tsf=0000000000000000
     * ie=000b6a6b6d2070726976617465010180dd180050f20101000050f20401000050f20401000050f2020000
     * ssid=jkm private
     */
    QString bss(int idx) const;

    /* BSS: 命令 "BSS <BSSID>" 获得详细的每个 BSS 扫描结果。
     */
    QString bss(const QString &bssid) const;

    /* ADD_NETWORK: 添加一个新的网络。此命令创建一个新网络，其配置为空。
     * 新网络被禁用，一旦配置好，就可以使用 ENABLE_NETWORK 命令启用它。
     * ADD_NETWORK 返回新网络的网络id，如果失败则返回 FAIL 。
     */
    QString add_network() const;

    /* REMOVE_NETWORK: 删除一个网络。可以从LIST_NETWORKS命令输出中接收网络id。
     * 特殊的网络 id(all) 都可以用来删除所有的网络。
     */
    QString remove_network(int id) const;

    /* SET_NETWORK: 设置网络变量。可以从 LIST_NETWORKS 命令输出中接收网络id。
     * 此命令使用与配置文件相同的变量和数据格式。有关详细信息，请参见示例wpa_supplicant.conf。
     * ssid (network name, SSID)
     * psk (WPA passphrase or pre-shared key)
     * key_mgmt (key management protocol)
     * identity (EAP identity)
     * password (EAP password)
     */
    QString set_network(int id, const QString &variable,
                        const QVariant &value) const;

    /* GET_NETWORK: 得到网络变量。可以从 LIST_NETWORKS 命令输出中接收网络id。
     */
    QString get_network(int id, const QString &variable) const;

    /* SELECT_NETWORK: 选择一个网络(禁用其他网络)。可以从 LIST_NETWORKS 命令输出中接收网络id。
     */
    QString select_network(int id) const;

    /* ENABLE_NETWORK: 启用网络。可以从LIST_NETWORKS命令输出中接收网络id。
     * 特殊的网络 id(all) 均可用于启用所有网络
     */
    QString enable_network(int id) const;

    /* DISABLE_NETWORK: 禁用网络。可以从LIST_NETWORKS命令输出中接收网络id。
     * 特殊的网络 id(all) 可以用来禁用所有网络。
     */
    QString disable_network(int id) const;

    /* SAVE_CONFIG: 保存当前配置。
     */
    QString save_config() const;

    /* P2P_FIND: 启动P2P设备发现。
     * 可选参数可用于指定以秒为单位的发现持续时间(例如，"P2P_FIND 5")。
     * 如果没有指定持续时间，将无限期地启动发现，即直到它被 P2P_STOP_FIND 或
     * P2P_CONNECT 终止(以使用发现的对等点开始组的形成)。
     *
     * 默认的搜索类型是首先对所有频道进行全面扫描，然后继续只扫描 social 频道(1,6,11)。
     * 可以通过指定不同的搜索类型来更改此行为: social (例如，"P2P_FIND 5 type=social")
     * 将跳过初始的完整扫描，只搜索 social 通道;
     * progressive (例如，"P2P_FIND type=progressive")从一个完整的扫描开始，
     * 然后使用 social 通道扫描在所有通道中逐步搜索一个通道。
     * progressive 设备发现可用于随着时间的推移查找新的组(以及在初始扫描期间没有找到的组，
     * 例如，由于GO处于睡眠状态)，而不会为每一轮搜索状态增加相当多的额外延迟。
     */
    QString p2p_find(int duration = 0, const QString &type = QString()) const;

    /* P2P_STOP_FIND: 停止正在进行的P2P设备发现或其他操作(连接、监听模式)。
     */
    QString p2p_stop_find() const;

    /* P2P_CONNECT: 启动 P2P 配对过程。
     * 这包括组所有者协商、设置配对组、准备和建立数据连接。
     */
    QString p2p_connect(const QString &address,
                        const QString &method,
                        const QString &option = QString()) const;

    /* P2P_LISTEN: 开始只接收状态。
     * 可选参数可用于指定监听操作的持续时间(以秒为单位)。
     * 此命令在正常操作期间可能没有多大用处，主要用于测试。
     * 它还可以用来保持设备的可发现性，而不需要维护组。
     */
    QString p2p_listen(int duration = 0) const;

    /* P2P_GROUP_ADD: 手动设置 P2P 组所有者(即而不与特定的对等方进行组所有者协商)。
     * 这也被称为主动 GO 。
     * P2P_GROUP_ADD [persistent] [freq=<in MHz>]
     * 可选参数 persistent=<network id> 可用于指定持久性组的重启。
     * 可选参数 freq 是组的信道频率(MHz)。
     */
    QString p2p_group_add(int persistent = -1, int freq = 0) const;

    /* P2P_GROUP_REMOVE: 终止P2P组。如果组使用了一个新的虚拟网络接口，那么它也将被删除。
     * 组接口的网络接口名用作此命令的参数。
     * P2P_GROUP_REMOVE <ifname>
     * <ifname> 是组接口的网络接口名或“*”全部删除。
     */
    QString p2p_group_remove(const QString &ifname) const;

    /* P2P_PEER: 获取关于已发现对等点的信息。显示关于已知对等点的信息。
     * P2P_PEER [addr] [FIRST] [NEXT-<P2P Device Address>]
     * 该命令接受一个参数，指定要选择哪个对等点:对等点的对等点设备地址，
     * "FIRST" 表示列表中的第一个对等点，或者 "NEXT-<P2P Device Address>"
     * 表示指定对等点之后的条目(允许遍历列表)。
     */
    QString p2p_peer() const;
    QString p2p_peers() const;

    /* P2P_INVITE: 邀请一个对等点加入一个组或(重新)启动一个持久组。
     * P2P_INVITE <cmd> [peer=addr]
     * <cmd> format is:
     * <persistent=id> [peer=addr] 要重新启动持久组，其中id是惟一的网络标识符
     * <group= > <peer=addr> [go_dev_addr=addr] 邀请peer加入一个活动组
     * 例如:
     * P2P_INVITE persistent=0 peer=11:22:33:44:55:66
     * P2P_INVITE group=wlan0 peer=11:22:33:44:55:66
     */
    QString p2p_invite(int persistent, const QString &peer = QString()) const;
    QString p2p_invite(const QString &group, const QString &peer = QString()) const;

    /* P2P_REJECT: 拒绝来自对等点的连接尝试(使用设备地址指定)。
     * 这是一种机制，用于拒绝与对等方进行的挂起GO协商，并请求自动阻止该对等方的任何进一步连接或发现。
     */
    QString p2p_reject() const;

public slots:
    void start();
    void stop();

signals:
    void supplicantStarted();
    void supplicantFinished();
    void messageReceived(const QString &msg);

private:
    explicit WiFiSupplicantTool(QObject *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(WiFiSupplicantTool)

    Q_PRIVATE_SLOT(d_func(), void _q_startSupplicantDone())
    Q_PRIVATE_SLOT(d_func(), void _q_stopSupplicantDone(int, QProcess::ExitStatus))
    Q_PRIVATE_SLOT(d_func(), void _q_supplicantError(QProcess::ProcessError))
};

class WiFiSupplicantToolPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(WiFiSupplicantTool)
public:
    WiFiSupplicantToolPrivate();
    ~WiFiSupplicantToolPrivate();

    void startSupplicant();
    void stopSupplicant();
    void _q_startSupplicantDone();
    void _q_stopSupplicantDone(int exitCode, QProcess::ExitStatus exitStatus);
    void _q_supplicantError(QProcess::ProcessError error);

    void wpaProcessMsg(char *msg);
    void wpaMonitorMsg();

    bool wpaOpenConnection();
    bool wpaCloseConnection();
    QString wpaCtrlRequest(const QString &command) const;

    QTimer *m_tryOpenTimer = NULL;
    QProcess *m_wpaProcess = NULL;
    QSocketNotifier *m_wpaMonitor = NULL;

    QString m_interface;
    QString m_interfacePath;

    struct wpa_ctrl *ctrl_conn = NULL;
    struct wpa_ctrl *monitor_conn = NULL;
};

QT_END_NAMESPACE

#endif // WIFISUPPLICANTTOOL_P_H
