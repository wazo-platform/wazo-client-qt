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

#ifndef _CURRENTCALLWIDGET_H_
#define _CURRENTCALLWIDGET_H_

#include "pendingwidget.h"

class CurrentCallWidget: public PendingWidget
{

Q_OBJECT

public:
    CurrentCallWidget(const QString &, const QString &, int, QWidget *);
    ~CurrentCallWidget();
    void update();
    bool toRemove() const;
    int line() const;
    const QString & channelid() const;
public slots:
    void doPickup() { }; // Do nothing
    void doAtxfer();
    void doConf();
    void doHangup();
    void doHold();
    void doPark();
    void doTxfer();
signals:
    void atxfer();
    // void blindTransfer(const QString &, int, const QString &, const QString &);
    void hangup();
    void hold();
    void park();
    void selectLine(int);
    void remove_me();
    void txfer();
    void conf();
protected:
    void buildui();
private:
    int m_line; //!< The phone's line number
    QString m_chan_xid;
    QPushButton * m_btn_atxfer;
    QPushButton * m_btn_txfer;
    QPushButton * m_btn_hold;
    QPushButton * m_btn_hangup;
    QPushButton * m_btn_conf;
    QPushButton * m_btn_park;
};

inline int CurrentCallWidget::line() const
{
    return m_line;
}

inline const QString & CurrentCallWidget::channelid() const
{
    return m_chan_xid;
}

#endif /* _CURRENTCALLWIDGET_H_ */
