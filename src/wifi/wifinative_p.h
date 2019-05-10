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

#ifndef WIFINATIVE_P_H
#define WIFINATIVE_P_H

#include "wifinative.h"
#include "wifisupplicantparser_p.h"
#include "wifisupplicanttool_p.h"

#include <private/qobject_p.h>
#include <QtCore/qtimer.h>
#include <QtCore/qmap.h>

class WiFiNativePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(WiFiNative)
public:
    WiFiNativePrivate();
    ~WiFiNativePrivate();

    void onSupplicantStarted();
    void onSupplicantFinished();
    void onMessageReceived(const QString &msg);

    void _q_updateInfoTimeout();
    void _q_autoScanTimeout();
    void _q_connNetTimeout();

    int addNetwork(const WiFiNetwork &network);
    int editNetwork(const WiFiNetwork &network);
    void selectNetwork(int networkId);
    void removeNetwork(int networkId);

    WiFiSupplicantParser parser;
    WiFiSupplicantTool *tool = NULL;
    QTimer *timer_Info = NULL;
    QTimer *timer_Scan = NULL;
    QTimer *timer_ConnNet = NULL;
    int timer_ConnNetId = -1;

    WiFi::State m_state = WiFi::StateDisabled;
    bool m_isAutoScan = false;
    WiFiInfo m_info;
    WiFiScanResultList m_scanResults;
    WiFiNetworkList m_networks;
    QMap<QString, int> m_netIdMapping;
};

#endif // WIFINATIVE_P_H
