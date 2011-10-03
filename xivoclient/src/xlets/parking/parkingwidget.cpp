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

#include "extendedlabel.h"
#include "extendedtablewidget.h"
#include "parkingwidget.h"

#include <QDebug>

/*! \brief create a parkinglot layout
 *
 * Create a simple layout containing a title and a list of parked call for the
 * tracked parkinglot
 * \param p The parking lot to track
 * \param parent The parent of this widget
 */
ParkingWidget::ParkingWidget(const QString & parkingid, QWidget * parent)
  :QWidget(parent), m_parking_id(parkingid)
{
    // qDebug() << Q_FUNC_INFO;
    const ParkingInfo * p = b_engine->parkinglot(parkingid);
    if (! p) return;
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setMargin(0);
    QString header = QString("%1 <%2> %3").arg(p->name()).arg(p->number()).arg(p->description());
    m_header = new ExtendedLabel(header, this);
    m_table = new ExtendedTableWidget(this);
    m_table->setAlternatingRowColors(true);
    m_table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    m_table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_table->setColumnCount(m_labels.size());
    m_table->setHorizontalHeaderLabels(m_labels);
    m_table->resizeColumnsToContents();
    layout->addWidget(m_header);
    layout->addWidget(m_table);
    connect(m_table, SIGNAL(itemClicked(QTableWidgetItem *)),
            this, SLOT(clickListener(QTableWidgetItem *)));
    connect(m_table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
            this, SLOT(doubleClickListener(QTableWidgetItem *)));
    connect(m_header, SIGNAL(clicked()), this, SLOT(headerClicked()));
    refresh();

    /*! \brief Columns header for the parked call tables */
    m_labels = QStringList() << tr("Number")
                             << tr("Time")
                             << tr("Parked")
                             << tr("Parker");
}

void ParkingWidget::headerClicked()
{
    const ParkingInfo * p = b_engine->parkinglot(m_parking_id);
    if (p) {
        emit parkinglotClicked(p->number());
    }
}

/*! \brief Refresh the widget table
 *
 * The widget table is updated with the current status of the parkinglot. This
 * method should not be used to update the time.
 */
void ParkingWidget::refresh()
{
    // qDebug() << Q_FUNC_INFO;
    removeUnparkedCalls();
    const ParkingInfo * p = b_engine->parkinglot(m_parking_id);
    if (p && p->countParked() > 0) {
        const QHash<QString, QVariant> & parking_bays = p->parkingBays();
        foreach (const QString key, parking_bays.keys()) {
            const QVariantMap & values = parking_bays[key].toMap();
            const QString & parker_channel = values["parker"].toString();
            const UserInfo * parker_ui = b_engine->getUserForXChannelId(QString("%0/%1").arg(p->ipbxid()).arg(parker_channel));
            QString parker;
            if (parker_ui) { // Blind transfer to the parking
                parker = parker_ui ? parker_ui->fullname() : QString();
            } else { // Attended transfer to the parking
                const QString & device_identity = parker_channel.split("-")[0];
                foreach (const XInfo * p, b_engine->iterover("phone")) {
                    if ((static_cast<const PhoneInfo *>(p))->identity() == device_identity) {
                        foreach (const XInfo * u, b_engine->iterover("user")) {
                            if ((static_cast<const UserInfo *>(u))->phonelist().contains(p->xid())) {
                                parker = (static_cast<const UserInfo *>(u))->fullname();
                            }
                        }
                    }
                }
            }
            QString exten = values["exten"].toString();
            if (! exten.isEmpty()) {
                QStringList l = QStringList() << exten
                                              << b_engine->timeElapsed(values["parktime"].toDouble())
                                              << values["cid_name"].toString()
                                              << parker;
                int row = findRow(exten);
                if (row < 0) {
                    addRow(l);
                } else {
                    setRow(row, l);
                }
            }
        }
    }
}

/*! \brief Remove unparked calls from the table */
void ParkingWidget::removeUnparkedCalls()
{
    // qDebug() << Q_FUNC_INFO << m_table->rowCount();
    const ParkingInfo * p = b_engine->parkinglot(m_parking_id);
    QList<int> toRemove;
    if (p && p->countParked() > 0) {
        for (int row = 0; row < m_table->rowCount(); ++row) {
            QString exten = m_table->item(row,0)->text();
            if (! p->parkingBays().contains(exten)) {
                toRemove.append(row);
            }
        }
    } else {
        for (int row = 0; row < m_table->rowCount(); ++row) {
            toRemove.append(row);
        }
    }
    foreach (int row, toRemove) {
        m_table->removeRow(row);
    }
}

/*! \brief return the number of parked calls tracked by this widget */
int ParkingWidget::count() const
{
    // qDebug() << Q_FUNC_INFO;
    const ParkingInfo * p = b_engine->parkinglot(m_parking_id);
    return p ? p->countParked() : 0;
}

/*! \brief Refresh the parking time in the table for each parked calls */
void ParkingWidget::refreshTimer()
{
    // qDebug() << Q_FUNC_INFO;
    const ParkingInfo * p = b_engine->parkinglot(m_parking_id);
    if (! p || p->parkingBays().size() < 1) return;
    const QHash<QString, QVariant> & parking_bays = p->parkingBays();
    for (int row = 0; row < m_table->rowCount(); ++row) {
        QTableWidgetItem * exten = m_table->item(row, 0);
        if (! exten) continue;
        QString key = exten->text();
        if (parking_bays.contains(key)) {
            const QVariantMap & values = parking_bays[key].toMap();
            QString new_time = b_engine->timeElapsed(values["parktime"].toDouble());
            QTableWidgetItem * time = m_table->takeItem(row, 1);
            time->setText(new_time);
            m_table->setItem(row, 1, time);
        }
    }
}

/*! \brief Add a new row to the widget table
 * \param list The strings that are going to be in the columns
 */
void ParkingWidget::addRow(const QStringList & list)
{
    // qDebug() << Q_FUNC_INFO << list;
    int row = m_table->rowCount();
    m_table->insertRow(row);
    setRow(row, list);
}

/*! \brief Set the content of a row to reflect the content of a string list
 * \param row The row to update
 * \param l The list of strings
 */
void ParkingWidget::setRow(int row, const QStringList & l)
{
    int col = 0;
    foreach (const QString val, l) {
        if (! val.isEmpty()) {
            QTableWidgetItem * item = new QTableWidgetItem(val);
            m_table->setItem(row, col++, item);
        } else {
            ++col;
        }
    }
}

/*! \brief Search for an extension in the table and return the row
 * \param exten The extension we are looking for
 * \return the row or -1
 */
int ParkingWidget::findRow(const QString & exten) const
{
    // qDebug() << Q_FUNC_INFO << exten;
    for (int row = 0; row < m_table->rowCount(); ++row) {
        const QTableWidgetItem * i = m_table->item(row,0);
        if (i->text() == exten) {
            return row;
        }
    }
    return -1;
}

void ParkingWidget::clickListener(QTableWidgetItem * i)
{
    // qDebug() << Q_FUNC_INFO;
    int row = m_table->row(i);
    QString exten = m_table->item(row,0)->text();
    emit itemClicked(exten);
}

void ParkingWidget::doubleClickListener(QTableWidgetItem * i)
{
    // qDebug() << Q_FUNC_INFO;
    int row = m_table->row(i);
    QString exten = m_table->item(row, 0)->text();
    const ParkingInfo * p = b_engine->parkinglot(m_parking_id);
    if (p) {
        emit itemDoubleClicked(p->ipbxid(), exten);
    }
}
