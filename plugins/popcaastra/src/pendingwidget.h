/* XiVO Client
 * Copyright (C) 2011, Proformatique
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

#ifndef __PENDINGWIDGET_H__
#define __PENDINGWIDGET_H__

#include <QDebug>
#include <QObject>
#include <QWidget>

class QLabel;
class QPushButton;
class QHBoxLayout;

/*!
 * A base widget class to track Pending calls that can be picked back by the
 * the operator.
 */
class PendingWidget : public QWidget
{

Q_OBJECT

public:
    /*! \brief Constructor
     *
     * \param phone_xid The tracked phone's XiVO id
     * \param parent
     */
    PendingWidget(const QString &, QWidget *);
    virtual ~PendingWidget() {};
    /*! \brief Refresh the widgets internal data and UI */
    virtual void update() = 0;
    /*! \brief Check if this widget is ready to be removed */
    virtual bool toRemove() const = 0;
    unsigned int id() const { return m_id; };
    const QString & phonexid() const;
public slots:
    /*! \brief Triggered action called when the calls is being picked up */
    virtual void doPickup() = 0;
signals:
    /*! \brief Notify the popc xlet that this widget should be removed */
    void remove_me(unsigned int);
protected:
    virtual void buildui(bool); //!< Creates inner widgets
    void set_string(const QString &);
    QString started_since() const;
    double time_start() const { return m_time_transfer; };
    QHBoxLayout * layout() { return m_layout; };
private:
    unsigned int m_id; //!< This call's internal id
    QString m_phonexid; //!< Tracked phone's XiVO id
    double m_time_transfer; //!< When we started tracking
    QHBoxLayout * m_layout; //!< The layout
    QLabel * m_lbl_string; //!< The info string for this call
    QPushButton * m_btn_pickup; //!< The button to pickup this call
    static unsigned int counted; //!< Used to assign an id to each instance
};

inline const QString & PendingWidget::phonexid() const
{
    return m_phonexid;
}

#endif

