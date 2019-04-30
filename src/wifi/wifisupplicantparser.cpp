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

#include "wifisupplicantparser_p.h"

#include <QtCore/qstring.h>
#include <QDebug>

WiFiSupplicantParser::WiFiSupplicantParser()
{
}


/*
    bssid=a4:50:46:78:0c:f6
    freq=2472
    ssid=ZZS
    id=0
    mode=station
    pairwise_cipher=CCMP
    group_cipher=CCMP
    key_mgmt=WPA2-PSK
    wpa_state=COMPLETED
    p2p_device_address=38:d2:69:c3:f8:3b
    address=38:d2:69:c3:f8:3b
    uuid=8feddb4f-154a-5190-94a1-5c6fe88c3d01
 */
WiFiInfo WiFiSupplicantParser::fromStatus(const QString &status) const
{
    WiFiInfo info;
    QStringList items = status.split(QRegExp(QStringLiteral("\\n")));
    for (int i = 0; i < items.size(); i++) {
        QString str = items.at(i);
        if (str.startsWith(QStringLiteral("address="))) {
            info.setMacAddress(WiFiMacAddress(str.section(QLatin1Char('='), 1)));
        } else if(str.startsWith(QStringLiteral("bssid="))) {
            info.setMacAddress(WiFiMacAddress(str.section(QLatin1Char('='), 1)));
        } else if(str.startsWith(QStringLiteral("ssid="))) {
            info.setSSID(str.section(QLatin1Char('='), 1));
        } else if(str.startsWith(QStringLiteral("freq="))) {
            bool ok;
            int freq = str.section(QLatin1Char('='), 1).trimmed().toInt(&ok);
            if(ok) {
                info.setFrequency(freq);
            }
        } else if(str.startsWith(QStringLiteral("ip_address="))) {
            info.setIpAddress(str.section(QLatin1Char('='), 1));
        } else if(str.startsWith(QStringLiteral("id="))) {
            bool ok;
            int id = str.section(QLatin1Char('='), 1).trimmed().toInt(&ok);
            if(ok) {
                info.setNetworkId(id);
            }
        }
    }
    return info;
}

/*
    id=138
    bssid=44:6e:e5:85:25:44
    freq=2437
    beacon_int=100
    capabilities=0x0431
    qual=0
    noise=-89
    level=-42
    tsf=0000650706200712
    age=7
    ie=000748494b2d595a32010882848b960c1218240301060706434e20010d212a010032043048606c2d1aee111bffffff00000000000000000001000000000000000000003d16060d06000000000000000000000000000000000000007f080000000000000040dd180050f202010180000153000027a4000042435e0062322f00dd0900037f01010000ff7fdd8b0050f204104a0001101044000102103b00010310470010123456789abcdef01234446ee58525441021000f48756177656920436f2e2c206c74641023000b576972656c657373204150102400033132331042000531323334351054000800060050f20400011011001248756177656920576972656c6573732041501008000206801049000600372a00012030140100000fac040100000fac040100000fac020c00dd0f00e0fc800000000100446ee5852544
    flags=[WPA2-PSK-CCMP][WPS][ESS]
    ssid=HIK-YZ2
    wps_state=configured
    wps_primary_device_type=6-0050F204-1
    wps_device_name=Huawei Wireless AP
    wps_config_methods=0x0680
    snr=47
    est_throughput=135000
 */
WiFiScanResult WiFiSupplicantParser::fromBSS(const QString &bss) const
{
    QString bssid, ssid, flags;
    qint16 rssi = WiFi::MIN_RSSI;
    int frequency = 0;
    QStringList items = bss.split(QRegExp(QStringLiteral("\\n")));
    for (int i = 0; i < items.size(); i++) {
        QString str = items.at(i);
        if (str.startsWith(QStringLiteral("bssid="))) {
            bssid = str.section(QLatin1Char('='), 1);
        } else if (str.startsWith(QStringLiteral("ssid="))) {
            ssid = str.section(QLatin1Char('='), 1);
        } else if (str.startsWith(QStringLiteral("level="))) {
            bool ok;
            short level = str.section(QLatin1Char('='), 1).trimmed().toShort(&ok);
            if(ok) {
                rssi = level;
            }
        } else if (str.startsWith(QStringLiteral("freq="))) {
            bool ok;
            int freq = str.section(QLatin1Char('='), 1).trimmed().toInt(&ok);
            if(ok) {
                frequency = freq;
            }
        } else if (str.startsWith(QStringLiteral("flags="))) {
            flags = str.section(QLatin1Char('='), 1);
        }
    }
    WiFiScanResult result(bssid, ssid);
    result.setRssi(rssi);
    result.setFrequency(frequency);
    result.setFlags(flags);
    return result;
}

WiFiNetworkList WiFiSupplicantParser::fromListNetworks(const QString &networks)
const
{
    WiFiNetworkList list;
    QStringList items = networks.split(QRegExp(QStringLiteral("\\n"))).mid(1);
    for (int i = 0; i < items.size(); i++) {
        QStringList net = items.at(i).split(QRegExp(QStringLiteral("\\t")));
        if(net.length() > 3) {
            int id = net.at(0).toInt();
            QString ssid = net.at(1);
            list << WiFiNetwork(id, ssid);
        }
    }
    return list;
}


WiFi::AuthFlags WiFiSupplicantParser::fromProtoKeyMgmt(const QString &proto,
        const QString &key_mgmt) const
{
    WiFi::AuthFlags auths = WiFi::NoneOpen;
    bool wpa2 = proto.contains(QStringLiteral("RSN")) ||
                proto.contains(QStringLiteral("WPA2"));
    if(key_mgmt.contains(QStringLiteral("WPA-EAP"))) {
        auths.setFlag(wpa2 ? WiFi::WPA2_EAP : WiFi::WPA_EAP);
    }
    if(key_mgmt.contains(QStringLiteral("WPA-PSK"))) {
        auths.setFlag(wpa2 ? WiFi::WPA2_PSK : WiFi::WPA_PSK);
    }
    if(key_mgmt.contains(QStringLiteral("IEEE8021X"))) {
        auths.setFlag(WiFi::IEEE8021X);
    }
    return auths;
}

WiFi::EncrytionFlags WiFiSupplicantParser::fromPairwise(
                const QString &pairwise) const
{
    WiFi::EncrytionFlags encrs = WiFi::None;
    if(pairwise.contains(QStringLiteral("WEP"))) {
        encrs.setFlag(WiFi::WEP);
    }
    if(pairwise.contains(QStringLiteral("TKIP"))) {
        encrs.setFlag(WiFi::TKIP);
    }
    if(pairwise.contains(QStringLiteral("CCMP"))) {
        encrs.setFlag(WiFi::CCMP);
    }
    return encrs;
}
