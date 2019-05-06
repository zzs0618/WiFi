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

#include "qquickwifisortfiltermodel_p.h"

#include <QtQml>

class QQuickWiFiSortFilterModelPrivate
{
    Q_DECLARE_PUBLIC(QQuickWiFiSortFilterModel)
public:
    QQuickWiFiSortFilterModelPrivate(QQuickWiFiSortFilterModel *p);
    virtual ~QQuickWiFiSortFilterModelPrivate();

    static int fuzzyCompare(float left, float right);
    static int fuzzyCompare(double left, double right);
    static qint64 variantCompare(const QVariant &left,
                                 const QVariant &right,
                                 Qt::CaseSensitivity cs, bool isLocaleAware);
public:
    QQuickWiFiSortFilterModel *q_ptr;
};

QQuickWiFiSortFilterModelPrivate::QQuickWiFiSortFilterModelPrivate(
                QQuickWiFiSortFilterModel *p)
    : q_ptr(p)
{
}

QQuickWiFiSortFilterModelPrivate::~QQuickWiFiSortFilterModelPrivate()
{
}

int QQuickWiFiSortFilterModelPrivate::fuzzyCompare(float left, float right)
{
    return qFuzzyCompare(left, right) ? 0 : (left < right ? -1 : 1);
}

int QQuickWiFiSortFilterModelPrivate::fuzzyCompare(double left, double right)
{
    return qFuzzyCompare(left, right) ? 0 : (left < right ? -1 : 1);
}

qint64 QQuickWiFiSortFilterModelPrivate::variantCompare(
                const QVariant &left,
                const QVariant &right,
                Qt::CaseSensitivity cs, bool isLocaleAware)
{
    if (left.userType() == QVariant::Invalid &&
        right.userType() == QVariant::Invalid) {
        return 0;
    } else if(left.userType() == QVariant::Invalid) {
        return 1;
    } else if (right.userType() == QVariant::Invalid) {
        return -1;
    }
    switch (left.userType()) {
        case QVariant::Int:
            return left.toInt() - right.toInt();
        case QVariant::UInt:
            return left.toInt() - right.toInt();
        case QVariant::LongLong:
            return left.toLongLong() - right.toLongLong();
        case QVariant::ULongLong:
            return left.toLongLong() - right.toLongLong();
        case QMetaType::Float:
            return fuzzyCompare(left.toFloat(), right.toFloat()) ;
        case QVariant::Double:
            return fuzzyCompare(left.toDouble(), right.toDouble()) ;
        case QVariant::Char:
            return left.toInt() - right.toInt();
        case QVariant::Date:
            return right.toDate().daysTo(left.toDate());
        case QVariant::Time:
            return  right.toTime().msecsTo(left.toTime());
        case QVariant::DateTime:
            return  right.toDateTime().msecsTo(left.toDateTime());
        case QVariant::String:
        default:
            if (isLocaleAware) {
                return left.toString().localeAwareCompare(right.toString());
            } else {
                return left.toString().compare(right.toString(), cs);
            }
    }
}


QQuickWiFiSortFilterModel::QQuickWiFiSortFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , d_ptr(new QQuickWiFiSortFilterModelPrivate(this))
    , m_complete(false)
{
    connect(this, &QSortFilterProxyModel::rowsInserted, this,
            &QQuickWiFiSortFilterModel::countChanged);
    connect(this, &QSortFilterProxyModel::rowsRemoved, this,
            &QQuickWiFiSortFilterModel::countChanged);
}

int QQuickWiFiSortFilterModel::count() const
{
    return rowCount();
}

QObject *QQuickWiFiSortFilterModel::source() const
{
    return sourceModel();
}

void QQuickWiFiSortFilterModel::setSource(QObject *source)
{
    setSourceModel(qobject_cast<QAbstractItemModel *>(source));
}

QByteArray QQuickWiFiSortFilterModel::sortRole() const
{
    return m_sortRole;
}

void QQuickWiFiSortFilterModel::setSortRole(const QByteArray &role)
{
    if (m_sortRole != role) {
        m_sortRole = role;
        if (m_complete) {
            QSortFilterProxyModel::setSortRole(roleKey(role));
        }
    }
}

void QQuickWiFiSortFilterModel::setSortOrder(Qt::SortOrder order)
{
    QSortFilterProxyModel::sort(0, order);
}

QByteArray QQuickWiFiSortFilterModel::filterRole() const
{
    return m_filterRole;
}

void QQuickWiFiSortFilterModel::setFilterRole(const QByteArray &role)
{
    if (m_filterRole != role) {
        m_filterRole = role;
        if (m_complete) {
            QSortFilterProxyModel::setFilterRole(roleKey(role));
        }
    }
}

QString QQuickWiFiSortFilterModel::filterString() const
{
    return filterRegExp().pattern();
}

void QQuickWiFiSortFilterModel::setFilterString(const QString &filter)
{
    setFilterRegExp(QRegExp(filter, filterCaseSensitivity(),
                            static_cast<QRegExp::PatternSyntax>(filterSyntax())));
}

QQuickWiFiSortFilterModel::FilterSyntax
QQuickWiFiSortFilterModel::filterSyntax() const
{
    return static_cast<FilterSyntax>(filterRegExp().patternSyntax());
}

void QQuickWiFiSortFilterModel::setFilterSyntax(
                QQuickWiFiSortFilterModel::FilterSyntax
                syntax)
{
    setFilterRegExp(QRegExp(filterString(), filterCaseSensitivity(),
                            static_cast<QRegExp::PatternSyntax>(syntax)));
}

QJSValue QQuickWiFiSortFilterModel::get(int idx) const
{
    QJSEngine *engine = qmlEngine(this);
    QJSValue value = engine->newObject();
    if (idx >= 0 && idx < count()) {
        QHash<int, QByteArray> roles = roleNames();
        QHashIterator<int, QByteArray> it(roles);
        while (it.hasNext()) {
            it.next();
            value.setProperty(QString::fromLocal8Bit(it.value()), data(index(idx, 0),
                              it.key()).toString());
        }
    }
    return value;
}

void QQuickWiFiSortFilterModel::classBegin()
{
}

void QQuickWiFiSortFilterModel::componentComplete()
{
    m_complete = true;
    if (!m_sortRole.isEmpty()) {
        QSortFilterProxyModel::setSortRole(roleKey(m_sortRole));
    }
    if (!m_filterRole.isEmpty()) {
        QSortFilterProxyModel::setFilterRole(roleKey(m_filterRole));
    }
}

int QQuickWiFiSortFilterModel::roleKey(const QByteArray &role) const
{
    QHash<int, QByteArray> roles = roleNames();
    QHashIterator<int, QByteArray> it(roles);
    while (it.hasNext()) {
        it.next();
        if (it.value() == role) {
            return it.key();
        }
    }
    return -1;
}

QHash<int, QByteArray> QQuickWiFiSortFilterModel::roleNames() const
{
    if (QAbstractItemModel *source = sourceModel()) {
        return source->roleNames();
    }
    return QHash<int, QByteArray>();
}

bool QQuickWiFiSortFilterModel::filterAcceptsRow(int sourceRow,
        const QModelIndex &sourceParent) const
{
    QRegExp rx = filterRegExp();
    if (rx.isEmpty()) {
        return true;
    }
    QAbstractItemModel *model = sourceModel();
    if (filterRole().isEmpty()) {
        QHash<int, QByteArray> roles = roleNames();
        QHashIterator<int, QByteArray> it(roles);
        while (it.hasNext()) {
            it.next();
            QModelIndex sourceIndex = model->index(sourceRow, 0, sourceParent);
            QString key = model->data(sourceIndex, it.key()).toString();
            if (key.contains(rx)) {
                return true;
            }
        }
        return false;
    }
    QModelIndex sourceIndex = model->index(sourceRow, 0, sourceParent);
    if (!sourceIndex.isValid()) {
        return true;
    }
    QString key = model->data(sourceIndex, roleKey(filterRole())).toString();
    return key.contains(rx);
}

//From QSortFilterProxyModel
bool QQuickWiFiSortFilterModel::lessThan(const QModelIndex &source_left,
        const QModelIndex &source_right) const
{
    QList<QByteArray> sortRoles = m_sortRole.split(',');

    if(sortRoles.isEmpty()) {
        return source_left.row() < source_right.row();
    }

    for(const QByteArray &name : sortRoles) {
        int role = roleNames().key(name);
        QVariant l = (source_left.model() ? source_left.model()->data(source_left,
                      role) : QVariant());
        QVariant r = (source_right.model() ? source_right.model()->data(source_right,
                      role) : QVariant());
        qint64 compare = QQuickWiFiSortFilterModelPrivate::variantCompare(l, r,
                         sortCaseSensitivity(), isSortLocaleAware());
        if(compare < 0) {
            return true;
        } else if(compare > 0) {
            return false;
        }
    }
    return false;
}
