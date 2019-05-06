/**
 ** This file is part of the WifiClient project.
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

#ifndef QQUICKWIFISORTFILTERMODEL_P_H
#define QQUICKWIFISORTFILTERMODEL_P_H

#include <QtQml/qqml.h>
#include <QtCore/qsortfilterproxymodel.h>
#include <QtQml/qqmlparserstatus.h>
#include <QtQml/qjsvalue.h>

class QQuickWiFiSortFilterModelPrivate;
class QQuickWiFiSortFilterModel : public QSortFilterProxyModel,
    public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QObject *source READ source WRITE setSource)

    Q_PROPERTY(QByteArray sortRole READ sortRole WRITE setSortRole)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder)

    Q_PROPERTY(QByteArray filterRole READ filterRole WRITE setFilterRole)
    Q_PROPERTY(QString filterString READ filterString WRITE setFilterString)
    Q_PROPERTY(FilterSyntax filterSyntax READ filterSyntax WRITE setFilterSyntax)

    Q_ENUMS(FilterSyntax)

public:
    explicit QQuickWiFiSortFilterModel(QObject *parent = 0);

    QObject *source() const;
    void setSource(QObject *source);

    QByteArray sortRole() const;
    void setSortRole(const QByteArray &role);

    void setSortOrder(Qt::SortOrder order);

    QByteArray filterRole() const;
    void setFilterRole(const QByteArray &role);

    QString filterString() const;
    void setFilterString(const QString &filter);

    enum FilterSyntax {
        RegExp,
        Wildcard,
        FixedString
    };

    FilterSyntax filterSyntax() const;
    void setFilterSyntax(FilterSyntax syntax);

    int count() const;
    Q_INVOKABLE QJSValue get(int index) const;

    void classBegin();
    void componentComplete();

signals:
    void countChanged();

protected:
    int roleKey(const QByteArray &role) const;
    QHash<int, QByteArray> roleNames() const;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

    //From QSortFilterProxyModel
    bool lessThan(const QModelIndex &source_left,
                  const QModelIndex &source_right) const Q_DECL_OVERRIDE;

protected:
    QQuickWiFiSortFilterModelPrivate *d_ptr;

private:
    Q_DISABLE_COPY(QQuickWiFiSortFilterModel)
    Q_DECLARE_PRIVATE(QQuickWiFiSortFilterModel)
    bool m_complete;
    QByteArray m_sortRole;
    QByteArray m_filterRole;
};

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QQuickWiFiSortFilterModel))

#endif // QQUICKWIFISORTFILTERMODEL_P_H
