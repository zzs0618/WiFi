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

#include "qquickwifiscanresultmodel_p.h"

#include <WiFi/wifimacaddress.h>

QQuickWiFiScanResultModel::QQuickWiFiScanResultModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_manager(new WiFiManager(this))
    , m_complete(false)
{
    connect(m_manager, &WiFiManager::scanResultFound, [this](const WiFiScanResult &result) {
        int i = m_scanResults.size();
        this->beginInsertRows(QModelIndex(), i, i);
        m_scanResults << result;
        this->endInsertRows();
    });
    connect(m_manager, &WiFiManager::scanResultUpdated, [this](const WiFiScanResult &result) {
        int i = m_scanResults.indexOf(result);
        m_scanResults.replace(i, result);
        this->dataChanged(index(i), index(i));
    });
    connect(m_manager, &WiFiManager::scanResultLost, [this](const WiFiScanResult &result) {
        int i = m_scanResults.indexOf(result);
        if(i > -1) {
            this->beginRemoveRows(QModelIndex(), i, i);
            m_scanResults.removeAt(i);
            this->endRemoveRows();
        }
    });
    connect(m_manager, &WiFiManager::scanResultsChanged, [this]() {
        this->beginResetModel();
        m_scanResults =  m_manager->scanResults();
        this->endResetModel();
    });

    connect(m_manager, SIGNAL(networkConnecting(int)),
            SLOT(onNetworkConnecting(int)));
    connect(m_manager, SIGNAL(networkAuthenticated(int)),
           SLOT(onNetworkAuthenticated(int)));
    connect(m_manager, SIGNAL(networkConnected(int)),
            SLOT(onNetworkConnected(int)));
    connect(m_manager, SIGNAL(networkErrorOccurred(int)),
            SLOT(onNetworkErrorOccurred(int)));

    m_scanResults =  m_manager->scanResults();
}

void QQuickWiFiScanResultModel::onNetworkConnecting(int networkId)
{
    m_status = qMakePair(networkId, 1);
    for(int i = 0; i < m_scanResults.size(); ++i) {
        if(m_scanResults[i].networkId() == networkId) {
            this->dataChanged(index(i), index(i));
        }
    }
}

void QQuickWiFiScanResultModel::onNetworkAuthenticated(int networkId)
{
    m_status = qMakePair(networkId, 2);
    for(int i = 0; i < m_scanResults.size(); ++i) {
        if(m_scanResults[i].networkId() == networkId) {
            this->dataChanged(index(i), index(i));
        }
    }
}

void QQuickWiFiScanResultModel::onNetworkConnected(int networkId)
{
    m_status = qMakePair(networkId, 3);
    for(int i = 0; i < m_scanResults.size(); ++i) {
        if(m_scanResults[i].networkId() == networkId) {
            this->dataChanged(index(i), index(i));
        }
    }
}

void QQuickWiFiScanResultModel::onNetworkErrorOccurred(int networkId)
{
    m_status = qMakePair(networkId, 4);
    for(int i = 0; i < m_scanResults.size(); ++i) {
        if(m_scanResults[i].networkId() == networkId) {
            this->dataChanged(index(i), index(i));
        }
    }
}

int QQuickWiFiScanResultModel::rowCount(const QModelIndex &) const
{
    return m_scanResults.count();
}

QVariant QQuickWiFiScanResultModel::data(const QModelIndex &index,
        int role) const
{
    if (!index.isValid() || index.row() < 0) {
        return QVariant();
    }

    if (index.row() >= m_scanResults.count()) {
        qWarning() << "WifiAccessPointModel: Index out of bound";
        return QVariant();
    }

    const WiFiScanResult &scanResult = m_scanResults.value(index.row());
    QVariantMap scanResultMap = scanResult.toMap();
    QVariant value;
    switch (role) {
        case Qt::DisplayRole + 7: {
            int rssi = scanResultMap["rssi"].toInt();
            value = WiFiManager::CalculateSignalLevel(rssi, 4);
        }
        break;
        case Qt::DisplayRole + 8: {
            WiFiMacAddress bssid = scanResult.bssid();
            int networkId = scanResult.networkId();
            if(m_manager->connectionInfo().bssid() == bssid) {
                value = 2;
            } else {
                value = (networkId >= 0) ? 1 : 0;    // 2: Current, 1: Network, 0: ScanResult
            }
        }
        break;
        case Qt::DisplayRole + 9: {
            if(scanResult.networkId() == m_status.first) {
                value = m_status.second;
            }else{
                value = 0;
            }
        }
        break;
        default:
            value = scanResultMap.value(roleNames().value(role));
            break;
    }
    return value;
}

QHash<int, QByteArray> QQuickWiFiScanResultModel::roleNames() const
{
    static QHash<int, QByteArray> roles = {
        {Qt::DisplayRole + 1, "ssid"},
        {Qt::DisplayRole + 2, "bssid"},
        {Qt::DisplayRole + 3, "rssi"},
        {Qt::DisplayRole + 4, "frequency"},
        {Qt::DisplayRole + 5, "flags"},
        {Qt::DisplayRole + 6, "networkId"},
        {Qt::DisplayRole + 7, "signalLevel"},
        {Qt::DisplayRole + 8, "type"},
        {Qt::DisplayRole + 9, "status"}
    };

    return roles;
}

void QQuickWiFiScanResultModel::classBegin()
{
}

void QQuickWiFiScanResultModel::componentComplete()
{
    m_complete = true;
}
