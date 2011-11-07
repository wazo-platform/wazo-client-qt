/* XiVO Client
 * Copyright (C) 2007-2011, Proformatique
 *
 * This file is part of XiVO Client.
 *
 * XiVO Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with a Section 7 Additional
 * Permission as follows:
 *   This notice constitutes a grant of such permission as is necessary
 *   to combine or link this software, or a modified version of it, with
 *   the OpenSSL project's "OpenSSL" library, or a derivative work of it,
 *   and to copy, modify, and distribute the resulting work. This is an
 *   extension of the special permission given by Trolltech to link the
 *   Qt code with the OpenSSL library (see
 *   <http://doc.trolltech.com/4.4/gpl.html>). The OpenSSL library is
 *   licensed under a dual license: the OpenSSL License and the original
 *   SSLeay license.
 *
 * XiVO Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <baseengine.h>

#include "queuessortfilterproxymodel.h"
#include "queuesmodel.h"

QueuesSortFilterProxyModel::QueuesSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    connect(b_engine, SIGNAL(settingsChanged()),
            this, SLOT(updateFilter()));
}

/*! \brief filter list setter
 */
void QueuesSortFilterProxyModel::setFilterId(const QString &xqueueid, bool filtered)
{
    if (filtered) {
        if (! m_filtered.contains(xqueueid)) {
            m_filtered.append(xqueueid);
        }
    } else {
        if (m_filtered.contains(xqueueid)) {
            m_filtered.removeAll(xqueueid);
        }
    }
}

bool QueuesSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex id_modelIndex = sourceModel()->index(sourceRow, QueuesModel::ID, sourceParent);
    QString id_string = sourceModel()->data(id_modelIndex).toString();
    return ! m_filtered.contains(id_string);
}

/*! \brief Update the filter list from config
 */
void QueuesSortFilterProxyModel::updateFilter()
{
    m_filtered.clear();
    /*! \todo review how and when the filter settings are stored :
     * - "visible" + queueid is a terrible index.
     * - They do not have to be transmitted to other classes, so we do not have
     * to go through BaseEngine for each modification.
     * Simply save and restore at opening/closure of the XLet should be enough.
     */
    QVariantMap filter = b_engine->getConfig("guioptions.queuespanel").toMap();
    foreach (QString key, filter.keys()) {
        if (key.startsWith("visible") && filter[key].toBool() == false) {
            key.remove(0, 7);
            setFilterId(key, true);
        }
    }
    emit layoutChanged();
}
