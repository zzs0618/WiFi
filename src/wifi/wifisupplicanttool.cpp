/**
 ** This file is part of the WifiHelper project.
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

#include "wifisupplicanttool_p.h"

extern "C"
{
#include "common/wpa_ctrl.h"
#include "utils/os.h"
#include "utils/common.h"
}

// in one source file
Q_LOGGING_CATEGORY(logWPA, "wifi.wpa.tool", QtInfoMsg)
Q_LOGGING_CATEGORY(logWPASupp, "wifi.wpa.supp", QtInfoMsg)

QT_BEGIN_NAMESPACE

static QByteArray WIFI_WPA_INTERFACE_DIR = "/var/run/wpa_supplicant";
static QByteArray WIFI_WPA_INTERFACE = "wlan0";
static QByteArray WIFI_WPA_COMMAND =
                "wpa_supplicant -c /etc/wpa_supplicant.conf";
static QByteArray WIFI_WPA_ACTION_DHCPC = "/sbin/dhcpc_action.sh";
static QByteArray WIFI_WPA_ACTION_DHCPD = "/sbin/dhcpd_action.sh";


WiFiSupplicantToolPrivate::WiFiSupplicantToolPrivate()
    : QObjectPrivate()
{
    if(!qEnvironmentVariableIsEmpty("WIFI_WPA_INTERFACE_DIR")) {
        WIFI_WPA_INTERFACE_DIR = qgetenv("WIFI_WPA_INTERFACE_DIR");
    }
    if(!qEnvironmentVariableIsEmpty("WIFI_WPA_INTERFACE")) {
        WIFI_WPA_INTERFACE = qgetenv("WIFI_WPA_INTERFACE");
    }
    if(!qEnvironmentVariableIsEmpty("WIFI_WPA_COMMAND")) {
        WIFI_WPA_COMMAND = qgetenv("WIFI_WPA_COMMAND");
    }
    if(!qEnvironmentVariableIsEmpty("WIFI_WPA_ACTION_DHCPC")) {
        WIFI_WPA_ACTION_DHCPC = qgetenv("WIFI_WPA_ACTION_DHCPC");
    }
    if(!qEnvironmentVariableIsEmpty("WIFI_WPA_ACTION_DHCPD")) {
        WIFI_WPA_ACTION_DHCPD = qgetenv("WIFI_WPA_ACTION_DHCPD");
    }

    m_interface = QString::fromLocal8Bit(WIFI_WPA_INTERFACE);
    m_interfacePath = QString(QStringLiteral("%1/%2")).arg(QString::fromLocal8Bit(
                                      WIFI_WPA_INTERFACE_DIR)).arg(m_interface);
}

WiFiSupplicantToolPrivate::~WiFiSupplicantToolPrivate()
{
    this->wpaCloseConnection();
}

void WiFiSupplicantToolPrivate::startSupplicant()
{
    Q_Q(WiFiSupplicantTool);

    if(!m_wpaProcess) {
        QStringList command = QString::fromLocal8Bit(WIFI_WPA_COMMAND).split(
                                              QLatin1Char(' '));
        QString program = command[0];
        QStringList arguments = command.mid(1);
        arguments << QStringLiteral("-i") << m_interface;

        m_wpaProcess = new QProcess(q);
        m_wpaProcess->setProgram(program);
        m_wpaProcess->setArguments(arguments);
        q->connect(m_wpaProcess, SIGNAL(readyReadStandardOutput()), q,
                   SLOT(_q_printSupplicantOutput()));
        q->connect(m_wpaProcess, SIGNAL(readyReadStandardError()), q,
                   SLOT(_q_printSupplicantError()));
        q->connect(m_wpaProcess, SIGNAL(started()), q,
                   SLOT(_q_startSupplicantDone()));
        q->connect(m_wpaProcess, SIGNAL(finished(int, QProcess::ExitStatus)), q,
                   SLOT(_q_stopSupplicantDone(int, QProcess::ExitStatus)));
        q->connect(m_wpaProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), q,
                   SLOT(_q_supplicantCrashed(QProcess::ProcessError)));
    }

    m_wpaProcess->start();
}

void WiFiSupplicantToolPrivate::stopSupplicant()
{
    if (m_wpaProcess) {
        if (m_wpaProcess->state() != QProcess::NotRunning) {
            m_wpaProcess->kill();
            m_wpaProcess->waitForFinished();
        }
    }
}

void WiFiSupplicantToolPrivate::_q_printSupplicantOutput()
{
    QTextStream stream(m_wpaProcess->readAllStandardOutput());
    QString line;
    while (stream.readLineInto(&line)) {
        qCDebug(logWPASupp, "[ OK ] %s", qUtf8Printable(line));
    }
}

void WiFiSupplicantToolPrivate::_q_printSupplicantError()
{
    QTextStream stream(m_wpaProcess->readAllStandardError());
    QString line;
    while (stream.readLineInto(&line)) {
        qCCritical(logWPASupp, "[FAIL] %s", qUtf8Printable(line));
    }
}

void WiFiSupplicantToolPrivate::_q_startSupplicantDone()
{
    Q_Q(WiFiSupplicantTool);

    if(!m_tryOpenTimer) {
        m_tryOpenTimer = new QTimer(q);
        m_tryOpenTimer->setInterval(50);
        m_tryOpenTimer->connect(m_tryOpenTimer, SIGNAL(timeout()), q,
                                SLOT(_q_tryOpenTimeout()));
    }

    m_tryOpenTimer->start();
}

void WiFiSupplicantToolPrivate::_q_stopSupplicantDone(int exitCode,
        QProcess::ExitStatus status)
{
    Q_Q(WiFiSupplicantTool);

    if (status != QProcess::NormalExit || exitCode != 0) {
        qCCritical(logWPA, "[FAIL] wpa_supplicant is crashed(%d).", exitCode);
    }

    this->wpaCloseConnection();
    Q_EMIT q->supplicantFinished();
}

void WiFiSupplicantToolPrivate::_q_supplicantCrashed(QProcess::ProcessError
        error)
{
    qCCritical(logWPA, "[FAIL] wpa_supplicant is Crashed.\n%s",
               qUtf8Printable(this->m_wpaProcess->errorString()));
    switch (error) {
        case QProcess::FailedToStart:
            break;
        case QProcess::Crashed:
            break;
        default:
            break;
    }
}

void WiFiSupplicantToolPrivate::_q_tryOpenTimeout()
{
    Q_Q(WiFiSupplicantTool);

    if(m_wpaProcess->state() == QProcess::Running) {
        if(wpaOpenConnection()) {
            qCInfo(logWPA, "[ OK ] Start wpa_supplicant successed.%s",
                   wifiPrintTimes((m_tryOpenTimes + 1) * m_tryOpenTimer->interval()));
            m_tryOpenTimer->stop();
            m_tryOpenTimes = 0;
            Q_EMIT q->supplicantStarted();
        } else if(m_tryOpenTimes < 100) {
            m_tryOpenTimes ++;
        } else {
            qCWarning(logWPA, "[FAIL] Start wpa_supplicant failed.%s",
                      wifiPrintTimes((m_tryOpenTimes + 1) * m_tryOpenTimer->interval()));
            m_tryOpenTimer->stop();
            m_tryOpenTimes = 0;
        }
    }
}

void WiFiSupplicantToolPrivate::wpaProcessMsg(char *msg)
{
    char *pos = msg;
    int priority = 2;

    if (*pos == '<') {
        /* skip priority */
        pos++;
        priority = atoi(pos);
        pos = strchr(pos, '>');
        if (pos) {
            pos++;
        } else {
            pos = msg;
        }
    }
    QString message = QString::fromLocal8Bit(pos);
    qCDebug(logWPA, "[ DEBUG ] WPA EVENT MSG<%d> : %s", priority,
            qUtf8Printable(message));

    if(message.startsWith(QStringLiteral(P2P_EVENT_GROUP_STARTED))) {
        QProcess::execute(QString::fromLocal8Bit(WIFI_WPA_ACTION_DHCPD),
                          QStringList() << m_interface << message);
    } else if(message.startsWith(QStringLiteral(P2P_EVENT_GROUP_REMOVED))) {
        QProcess::execute(QString::fromLocal8Bit(WIFI_WPA_ACTION_DHCPD),
                          QStringList() << m_interface << message);
    }

    Q_EMIT q_func()->messageReceived(message);
}

void WiFiSupplicantToolPrivate::wpaMonitorMsg()
{
    char buf[256];
    size_t len;

    while (monitor_conn && wpa_ctrl_pending(monitor_conn) > 0) {
        len = sizeof(buf) - 1;
        if (wpa_ctrl_recv(monitor_conn, buf, &len) == 0) {
            buf[len] = '\0';
            wpaProcessMsg(buf);
        }
    }
}

bool WiFiSupplicantToolPrivate::wpaOpenConnection()
{
    if(ctrl_conn) {
        return true;
    }

    ctrl_conn = wpa_ctrl_open(qPrintable(m_interfacePath));
    if (ctrl_conn == NULL) {
        return false;
    }

    monitor_conn = wpa_ctrl_open(qPrintable(m_interfacePath));
    if (monitor_conn == NULL) {
        wpa_ctrl_close(ctrl_conn);
        ctrl_conn = NULL;
        return false;
    }

    if (wpa_ctrl_attach(monitor_conn)) {
        wpa_ctrl_close(monitor_conn);
        monitor_conn = NULL;
        wpa_ctrl_close(ctrl_conn);
        ctrl_conn = NULL;
        return false;
    }

#if defined(CONFIG_CTRL_IFACE_UNIX) || defined(CONFIG_CTRL_IFACE_UDP)
    m_wpaMonitor = new QSocketNotifier(wpa_ctrl_get_fd(monitor_conn),
                                       QSocketNotifier::Read);
    QObjectPrivate::connect(m_wpaMonitor, &QSocketNotifier::activated, this,
                            &WiFiSupplicantToolPrivate::wpaMonitorMsg);
#endif

    return true;
}

bool WiFiSupplicantToolPrivate::wpaCloseConnection()
{
    if (ctrl_conn) {
        wpa_ctrl_close(ctrl_conn);
        ctrl_conn = NULL;
    }

    if (monitor_conn) {
        m_wpaMonitor->setEnabled(false);
        m_wpaMonitor->deleteLater();
        m_wpaMonitor = NULL;
        wpa_ctrl_detach(monitor_conn);
        wpa_ctrl_close(monitor_conn);
        monitor_conn = NULL;
    }

    return true;
}

QString WiFiSupplicantToolPrivate::wpaCtrlRequest(const QString &command) const
{
    int ret;
    const char *cmd = command.toLocal8Bit().constData();
    char buf[2048], decode[2048];
    size_t len;
    if (ctrl_conn == NULL) {
        qCCritical(logWPA, "[FAIL] Forbbiden to wpa_ctrl_request.\n%s",
                   qUtf8Printable(command));
        return QString();
    }

    len = sizeof(buf) - 1;
    ret = wpa_ctrl_request(ctrl_conn, cmd, strlen(cmd), buf, &len, NULL);

    if (ret == -2) {
        qCCritical(logWPA, "[FAIL] Timeout to wpa_ctrl_request.\n%s",
                   qUtf8Printable(command));
        return QString();
    } else if (ret < 0) {
        qCCritical(logWPA, "[FAIL] Failed to wpa_ctrl_request.\n%s",
                   qUtf8Printable(command));
        return QString();
    } else {
        buf[len] = '\0';
        printf_decode((u8 *)decode, sizeof(decode), buf);
    }

    return QString::fromLocal8Bit(decode);
}

WiFiSupplicantTool::WiFiSupplicantTool(QObject *parent)
    : QObject(*(new WiFiSupplicantToolPrivate), parent)
{
}

WiFiSupplicantTool *WiFiSupplicantTool::instance()
{
    static WiFiSupplicantTool *self = new WiFiSupplicantTool;
    return self;
}

QString WiFiSupplicantTool::ping() const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("PING");
    return d->wpaCtrlRequest(command); // "PONG\n"
}

QString WiFiSupplicantTool::status(bool verbose) const
{
    Q_D(const WiFiSupplicantTool);
    QString command = verbose ? QStringLiteral("STATUS-VERBOSE") :
                      QStringLiteral("STATUS");
    return d->wpaCtrlRequest(command);
}

QString WiFiSupplicantTool::reassociate() const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("REASSOCIATE");
    return d->wpaCtrlRequest(command); // "OK\n"
}

QString WiFiSupplicantTool::reconnect() const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("RECONNECT");
    return d->wpaCtrlRequest(command); // "OK\n"
}

QString WiFiSupplicantTool::disconnect() const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("DISCONNECT");
    return d->wpaCtrlRequest(command); // "OK\n"
}

QString WiFiSupplicantTool::reconfigure() const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("RECONFIGURE");
    return d->wpaCtrlRequest(command); // "OK\n"
}

QString WiFiSupplicantTool::terminate() const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("TERMINATE");
    return d->wpaCtrlRequest(command); // "OK\n"
}

QString WiFiSupplicantTool::list_networks() const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("LIST_NETWORKS");
    return d->wpaCtrlRequest(command);
    // "network id / ssid / bssid / flags\n0\texample network\tany\t[CURRENT]\n"
}

QString WiFiSupplicantTool::scan() const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("SCAN");
    return d->wpaCtrlRequest(command); // "OK\n" or "FAIL-BUSY\n"
}

QString WiFiSupplicantTool::scan_results() const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("SCAN_RESULTS");
    return d->wpaCtrlRequest(command);
    // "bssid / frequency / signal level / flags / ssid\n00:09:5b:95:e0:4e\t2412\t-40\t[WPA2-PSK-CCMP][WPS][ESS]\tjkm private\n"
}

QString WiFiSupplicantTool::bss(int idx) const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("BSS %1");
    command = command.arg(idx);
    return d->wpaCtrlRequest(command);
}

QString WiFiSupplicantTool::bss(const QString &bssid) const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("BSS %1");
    command = command.arg(bssid);
    return d->wpaCtrlRequest(command);
}

QString WiFiSupplicantTool::add_network() const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("ADD_NETWORK");
    QString result = d->wpaCtrlRequest(command); // "0\n"
    qCDebug(logWPA, "[ OK ] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    return result;
}

QString WiFiSupplicantTool::remove_network(int id) const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("REMOVE_NETWORK %1");
    if(id >= 0) {
        command = command.arg(id);
    } else {
        command = command.arg(QLatin1String("all"));
    }
    QString result = d->wpaCtrlRequest(command); // "OK\n" or "FAIL\n"
    if(result.startsWith(QStringLiteral("FAIL"))) {
        qCCritical(logWPA, "[FAIL] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }else{
        qCDebug(logWPA, "[ OK ] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }
    return result;
}

QString WiFiSupplicantTool::set_network(int id, const QString &variable,
                                        const QVariant &value) const
{
    Q_D(const WiFiSupplicantTool);
    static QString has_quotes = QStringLiteral("ssid,psk");
    QString command = QStringLiteral("SET_NETWORK %1 %2");
    QString param = QStringLiteral("%1 %2");
    param = param.arg(variable);
    if(has_quotes.contains(variable)) {
        param = param.arg(QLatin1String("\"") + value.toString() + QLatin1String("\""));
    } else {
        param = param.arg(value.toString());
    }
    command = command.arg(id).arg(param);
    QString result = d->wpaCtrlRequest(command); // "OK\n" or "FAIL\n"
    if(result.startsWith(QStringLiteral("FAIL"))) {
        qCCritical(logWPA, "[FAIL] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }else{
        qCDebug(logWPA, "[ OK ] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }
    return result;
}

QString WiFiSupplicantTool::get_network(int id, const QString &variable) const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("GET_NETWORK %1 %2");
    command = command.arg(id).arg(variable);
    return d->wpaCtrlRequest(command);
}

QString WiFiSupplicantTool::select_network(int id) const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("SELECT_NETWORK %1");
    command = command.arg(id);
    QString result = d->wpaCtrlRequest(command); // "OK\n" or "FAIL\n"
    if(result.startsWith(QStringLiteral("FAIL"))) {
        qCCritical(logWPA, "[FAIL] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }else{
        qCDebug(logWPA, "[ OK ] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }
    return result;
}

QString WiFiSupplicantTool::enable_network(int id) const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("ENABLE_NETWORK %1");
    if(id >= 0) {
        command = command.arg(id);
    } else {
        command = command.arg(QLatin1String("all"));
    }
    QString result = d->wpaCtrlRequest(command); // "OK\n" or "FAIL\n"
    if(result.startsWith(QStringLiteral("FAIL"))) {
        qCCritical(logWPA, "[FAIL] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }else{
        qCDebug(logWPA, "[ OK ] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }
    return result;
}

QString WiFiSupplicantTool::disable_network(int id) const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("DISABLE_NETWORK %1");
    if(id >= 0) {
        command = command.arg(id);
    } else {
        command = command.arg(QLatin1String("all"));
    }
    QString result = d->wpaCtrlRequest(command); // "OK\n" or "FAIL\n"
    if(result.startsWith(QStringLiteral("FAIL"))) {
        qCCritical(logWPA, "[FAIL] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }else{
        qCDebug(logWPA, "[ OK ] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }
    return result;
}

QString WiFiSupplicantTool::save_config() const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("SAVE_CONFIG");
    QString result = d->wpaCtrlRequest(command); // "OK\n" or "FAIL\n"
    if(result.startsWith(QStringLiteral("FAIL"))) {
        qCDebug(logWPA, "[FAIL] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }else{
        qCDebug(logWPA, "[ OK ] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }
    return result;
}

QString WiFiSupplicantTool::p2p_find(int duration, const QString &type) const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("P2P_FIND");
    if(duration > 0) {
        command += QString(tr(" %1")).arg(duration);
    }
    if(!type.isEmpty()) {
        command += QString(tr(" type=%1")).arg(type);
    }
    QString result = d->wpaCtrlRequest(command); // "OK\n" or "FAIL\n"
    if(result.startsWith(QStringLiteral("FAIL"))) {
        qCCritical(logWPA, "[FAIL] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }else{
        qCDebug(logWPA, "[ OK ] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }
    return result;
}

QString WiFiSupplicantTool::p2p_stop_find() const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("P2P_STOP_FIND");
    QString result = d->wpaCtrlRequest(command); // "OK\n" or "FAIL\n"
    if(result.startsWith(QStringLiteral("FAIL"))) {
        qCCritical(logWPA, "[FAIL] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }else{
        qCDebug(logWPA, "[ OK ] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }
    return result;
}

QString WiFiSupplicantTool::p2p_connect(const QString &address,
                                        const QString &method,
                                        const QString &option) const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("P2P_CONNECT %1 %2");
    command = command.arg(address).arg(method);
    if(!option.isEmpty()) {
        command += QString(tr(" %1")).arg(option);
    }
    QString result = d->wpaCtrlRequest(command); // "OK\n" or "FAIL\n"
    if(result.startsWith(QStringLiteral("FAIL"))) {
        qCCritical(logWPA, "[FAIL] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }else{
        qCDebug(logWPA, "[ OK ] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }
    return result;
}

QString WiFiSupplicantTool::p2p_listen(int duration) const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("P2P_LISTEN");
    if(duration > 0) {
        command += QString(tr(" %1")).arg(duration);
    }
    QString result = d->wpaCtrlRequest(command); // "OK\n" or "FAIL\n"
    if(result.startsWith(QStringLiteral("FAIL"))) {
        qCCritical(logWPA, "[FAIL] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }else{
        qCDebug(logWPA, "[ OK ] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }
    return result;
}

QString WiFiSupplicantTool::p2p_group_add(int persistent, int freq) const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("P2P_GROUP_ADD");
    if(persistent >= 0) {
        command += QString(tr(" persistent=%1")).arg(persistent);
    }
    if(freq > 0) {
        command += QString(tr(" freq=%1")).arg(freq);
    }
    QString result = d->wpaCtrlRequest(command); // "OK\n" or "FAIL\n"
    if(result.startsWith(QStringLiteral("FAIL"))) {
        qCCritical(logWPA, "[FAIL] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }else{
        qCDebug(logWPA, "[ OK ] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }
    return result;
}

QString WiFiSupplicantTool::p2p_group_remove(const QString &ifname) const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("P2P_GROUP_REMOVE %1");
    command = command.arg(ifname);
    QString result = d->wpaCtrlRequest(command); // "OK\n" or "FAIL\n"
    if(result.startsWith(QStringLiteral("FAIL"))) {
        qCCritical(logWPA, "[FAIL] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }else{
        qCDebug(logWPA, "[ OK ] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }
    return result;
}

QString WiFiSupplicantTool::p2p_peer() const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("P2P_PEER");
    QString result = d->wpaCtrlRequest(command); // "OK\n" or "FAIL\n"
    if(result.startsWith(QStringLiteral("FAIL"))) {
        qCCritical(logWPA, "[FAIL] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }else{
        qCDebug(logWPA, "[ OK ] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }
    return result;
}

QString WiFiSupplicantTool::p2p_peers() const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("P2P_PEERS");
    QString result = d->wpaCtrlRequest(command); // "OK\n" or "FAIL\n"
    if(result.startsWith(QStringLiteral("FAIL"))) {
        qCCritical(logWPA, "[FAIL] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }else{
        qCDebug(logWPA, "[ OK ] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }
    return result;
}

QString WiFiSupplicantTool::p2p_invite(int persistent,
                                       const QString &peer) const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("P2P_INVITE persistent=%1");
    command = command.arg(persistent);
    if(!peer.isEmpty()) {
        command += QString(tr(" peer=%1")).arg(peer);
    }
    QString result = d->wpaCtrlRequest(command); // "OK\n" or "FAIL\n"
    if(result.startsWith(QStringLiteral("FAIL"))) {
        qCCritical(logWPA, "[FAIL] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }else{
        qCDebug(logWPA, "[ OK ] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }
    return result;
}

QString WiFiSupplicantTool::p2p_invite(const QString &group,
                                       const QString &peer) const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("P2P_INVITE group=%1");
    command = command.arg(group);
    if(!peer.isEmpty()) {
        command += QString(tr(" peer=%1")).arg(peer);
    }
    QString result = d->wpaCtrlRequest(command); // "OK\n" or "FAIL\n"
    if(result.startsWith(QStringLiteral("FAIL"))) {
        qCCritical(logWPA, "[FAIL] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }else{
        qCDebug(logWPA, "[ OK ] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }
    return result;
}

QString WiFiSupplicantTool::p2p_reject() const
{
    Q_D(const WiFiSupplicantTool);
    QString command = QStringLiteral("P2P_REJECT");
    QString result = d->wpaCtrlRequest(command); // "OK\n" or "FAIL\n"
    if(result.startsWith(QStringLiteral("FAIL"))) {
        qCCritical(logWPA, "[FAIL] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }else{
        qCDebug(logWPA, "[ OK ] %s -> %s", qUtf8Printable(command), qUtf8Printable(result.trimmed()));
    }
    return result;
}

void WiFiSupplicantTool::dhcpc_request()
{
    Q_D(WiFiSupplicantTool);

    QProcess::execute(QString::fromLocal8Bit(WIFI_WPA_ACTION_DHCPC),
                      QStringList() << d->m_interface << QStringLiteral("CONNECTED"));
}

void WiFiSupplicantTool::dhcpc_release()
{
    Q_D(WiFiSupplicantTool);

    QProcess::execute(QString::fromLocal8Bit(WIFI_WPA_ACTION_DHCPC),
                      QStringList() << d->m_interface << QStringLiteral("DISCONNECTED"));
}

bool WiFiSupplicantTool::isRunning() const
{
    Q_D(const WiFiSupplicantTool);
    return d->ctrl_conn != NULL;
}

void WiFiSupplicantTool::start()
{
    Q_D(WiFiSupplicantTool);
    d->startSupplicant();
}

void WiFiSupplicantTool::stop()
{
    Q_D(WiFiSupplicantTool);
    d->stopSupplicant();
}

QT_END_NAMESPACE
