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

/* $Revision$
 * $Date$
 */

#include "operator.h"

Q_EXPORT_PLUGIN2(xletoperatorplugin, XLetOperatorPlugin);

XLet* XLetOperatorPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/operator_%1");
    return new XletOperator(parent);
}

XletOperator::XletOperator(QWidget * parent)
    : XLet(parent)
{
    m_glayout = new QGridLayout(this);
    m_lbl = new QLabel( "", this );
    setAccessibleName( tr("Operator panel") );
    setTitle( tr("Operator") );

    QVariantMap opts = b_engine->getGuiOptions("client_gui");

    m_actionkey[opts["xlet_operator_keyanswer"        ].toInt()] = (QStringList() << "answer" << tr("Answer"));
    m_actionkey[opts["xlet_operator_keyhangup"        ].toInt()] = (QStringList() << "hangup" << tr("Hangup"));
    m_actionkey[opts["xlet_operator_keydtransfer"     ].toInt()] = (QStringList() << "dtransfer" << tr("D. Transfer"));
    m_actionkey[opts["xlet_operator_keyitransfer"     ].toInt()] = (QStringList() << "itransfer" << tr("I. Transfer"));
    m_actionkey[opts["xlet_operator_keyilink"         ].toInt()] = (QStringList() << "ilink" << tr("I. Link"));
    m_actionkey[opts["xlet_operator_keyicancel"       ].toInt()] = (QStringList() << "icancel" << tr("I. Cancel"));
    m_actionkey[opts["xlet_operator_keypark"          ].toInt()] = (QStringList() << "park" << tr("Park"));
    m_actionkey[opts["xlet_operator_keyatxferfinalize"].toInt()] = (QStringList() << "atxferfinalize" << tr("Finalize Transfer"));
    m_actionkey[opts["xlet_operator_keyatxfercancel"  ].toInt()] = (QStringList() << "atxfercancel" << tr("Cancel Transfer"));
    // m_actionkey[Qt::Key_Return] = (QStringList() << "numreturn" << tr("Call Number"));

    m_glayout->addWidget( m_lbl, 0, 0, 1, m_actionkey.size() + 4, Qt::AlignHCenter | Qt::AlignVCenter );
    m_glayout->setRowStretch(100, 1);

    // connect signal/SLOTS
    connect(b_engine, SIGNAL(updateUserConfig(const QString &)),
            this, SLOT(updateUserConfig(const QString &)));
    connect(b_engine, SIGNAL(updateUserStatus(const QString &)),
            this, SLOT(updateUserStatus(const QString &)));
    connect(b_engine, SIGNAL(updatePhoneConfig(const QString &)),
            this, SLOT(updatePhoneConfig(const QString &)));
    connect(b_engine, SIGNAL(updatePhoneStatus(const QString &)),
            this, SLOT(updatePhoneStatus(const QString &)));
    connect(b_engine, SIGNAL(updateChannelStatus(const QString &)),
            this, SLOT(updateChannelStatus(const QString &)));
}

/*! \brief add a line of widgets for a call
 *
 * create all widgets and fill m_vlinesl, m_vlinesr, m_statuses, m_tnums
 */
void XletOperator::newCall(const QString & chan)
{
    m_vlinesl[chan] = new QFrame(this);
    m_vlinesr[chan] = new QFrame(this);
    m_statuses[chan] = new QLabel("none", this);
    m_tnums[chan] = new QLineEdit("", this);

    m_vlinesl[chan]->setFrameShape(QFrame::VLine);
    m_vlinesl[chan]->setLineWidth(1);
    m_vlinesr[chan]->setFrameShape(QFrame::VLine);
    m_vlinesr[chan]->setLineWidth(1);
    m_tnums[chan]->hide();

    changeCurrentChannel(m_currentchannel, chan);
    m_currentchannel = chan;

    QHash<QString, QPushButton *> k;
    QMapIterator<int, QStringList> act(m_actionkey);
    while (act.hasNext()) {
        act.next();
        QString actionname = act.value()[0];
        k[actionname] = new QPushButton("", this);
        k[actionname]->hide();

        k[actionname]->setText(act.value()[1] + " (" + QKeySequence(act.key()).toString() + ")");

        k[actionname]->setProperty("channel", chan);
        k[actionname]->setProperty("function", act.key());
        connect(k[actionname], SIGNAL(clicked()),
                this, SLOT(clicked()));
    }

    m_actions[chan] = k;
    int row = 1;
    foreach (int r, m_row) {
        if (r >= row) {
            row = r + 1;
        }
    }
    m_row[chan] = row;
}

/*! \brief update display of a line of buttons
 *
 * first clean the line by removing all buttons
 * then add wanted buttons.
 */
void XletOperator::updateLine(const QString & chan, const QStringList & allowed)
{
    int row = m_row[chan];
    int colnum = 1;
    qDebug() << Q_FUNC_INFO << row << "allowed=" << allowed;
    m_glayout->addWidget( m_vlinesl[chan], row, 0, Qt::AlignLeft );
    m_glayout->addWidget( m_statuses[chan], row, colnum++, Qt::AlignHCenter );

    QMapIterator<int, QStringList> act(m_actionkey);
    while (act.hasNext()) {
        act.next();
        QString actionname = act.value()[0];
        m_actions[chan][actionname]->hide();
        m_glayout->removeWidget(m_actions[chan][actionname]);
    }
    act.toFront();
    while (act.hasNext()) {
        act.next();
        QString actionname = act.value()[0];
        if (allowed.contains(actionname)) {
            m_glayout->addWidget( m_actions[chan][actionname], row, colnum++, Qt::AlignHCenter );
            m_actions[chan][actionname]->show();
        }
    }
    m_glayout->addWidget( m_tnums[chan],   row, m_actionkey.size() + 2, Qt::AlignHCenter );
    m_glayout->addWidget( m_vlinesr[chan], row, m_actionkey.size() + 3, Qt::AlignRight );
}

/*! \brief received when a button was pressed
 *
 * Simultate the press of a function key
 */
void XletOperator::clicked()
{
    QString channel = sender()->property("channel").toString();
    int function = sender()->property("function").toInt();
    qDebug() << Q_FUNC_INFO << channel << function;
    m_currentchannel = channel;
    functionKeyPressed(function);
}

/*! \brief setup things for a direct transfer
 */
void XletOperator::dtransfer()
{
    if (m_callchannels.contains(m_currentchannel)) {
        // qDebug() << Q_FUNC_INFO << "Direct Transfer" << m_currentchannel;
        if (m_linestatuses[m_currentchannel] == WDTransfer) {
            m_tnums[m_currentchannel]->hide();
            m_statuses[m_currentchannel]->setFocus();
            m_linestatuses[m_currentchannel] = Ringing;
            disconnect(m_tnums[m_currentchannel], SIGNAL(returnPressed()),
                       this, SLOT(xferPressed()));
        } else {
            m_tnums[m_currentchannel]->show();
            m_tnums[m_currentchannel]->setFocus();
            m_linestatuses[m_currentchannel] = WDTransfer;
            connect(m_tnums[m_currentchannel], SIGNAL(returnPressed()),
                    this, SLOT(xferPressed()));
        }
    }
}

/*! \brief set up things for an indirect transfer
 */
void XletOperator::itransfer()
{
    if (m_callchannels.contains(m_currentchannel)) {
        // qDebug() << Q_FUNC_INFO << "Indirect Transfer" << m_currentchannel;
        if (m_linestatuses[m_currentchannel] == WITransfer) {
            m_tnums[m_currentchannel]->hide();
            m_statuses[m_currentchannel]->setFocus();
            m_linestatuses[m_currentchannel] = Online;
            disconnect(m_tnums[m_currentchannel], SIGNAL(returnPressed()),
                       this, SLOT(xferPressed()) );
        } else {
            m_tnums[m_currentchannel]->show();
            m_tnums[m_currentchannel]->setFocus();
            m_linestatuses[m_currentchannel] = WITransfer;
            connect(m_tnums[m_currentchannel], SIGNAL(returnPressed()),
                    this, SLOT(xferPressed()));
        }
    }
}

/*! \brief Does the transfer
 */
void XletOperator::xferPressed()
{
    QString number = m_tnums[m_currentchannel]->text();
    QString peerchan = getPeerChan(m_currentchannel);
    QString dst = QString("ext:%1").arg(number);
    // qDebug() << Q_FUNC_INFO << m_currentchannel << peerchan << m_linestatuses[m_currentchannel] << number;

    if (m_linestatuses[m_currentchannel] == WDTransfer) {
        QString src = QString("chan:special:me:%1").arg(peerchan);
        b_engine->actionCall("transfer", src, dst);
    } else if (m_linestatuses[m_currentchannel] == WITransfer) {
        QString src = QString("chan:special:me:%1").arg(m_currentchannel);
        b_engine->actionCall("atxfer", src, dst);
        updateLine(m_currentchannel, (QStringList() << "hangup" << "ilink" << "icancel"));
    }
}

/*! \brief a key was pressed
 *
 * \todo make icancel work
 */
void XletOperator::functionKeyPressed(int keynum)
{
    // qDebug() << Q_FUNC_INFO << keynum << m_currentchannel;

    if (keynum == Qt::Key_Up) {
        if (m_currentchannel.isEmpty()) {
            return;
        }
        int ci = m_callchannels.indexOf(m_currentchannel);
        if (ci > 0)
            ci--;
        changeCurrentChannel(m_currentchannel, m_callchannels[ci]);
        m_currentchannel = m_callchannels[ci];
    } else if (keynum == Qt::Key_Down) {
        if (m_currentchannel.isEmpty()) {
            return;
        }
        int ci = m_callchannels.indexOf(m_currentchannel);
        if (ci < m_callchannels.size() - 1) {
            ci++;
        }
        changeCurrentChannel(m_currentchannel, m_callchannels[ci]);
        m_currentchannel = m_callchannels[ci];
    }

    QString action;
    if (m_actionkey.contains(keynum)) {
        action = m_actionkey[keynum][0];
    } else {
        return;
    }

    if (m_callchannels.contains(m_currentchannel)) {
        Line linestatus = m_linestatuses[m_currentchannel];
        qDebug() << Q_FUNC_INFO << keynum << action << m_currentchannel << linestatus;
        if (action == "answer") {
            b_engine->actionCall("answer", QString("chan:%1:not_relevant_here").arg(m_xphoneid));
        } else if (action == "hangup") {
            if (linestatus == Ringing || linestatus == WITransfer || linestatus == WDTransfer) {
                b_engine->actionCall("hangup", QString("chan:%1:%2").arg(m_xphoneid).arg(getPeerChan(m_currentchannel))); // Call
            } else {
                b_engine->actionCall("hangup", QString("chan:%1:%2").arg(m_xphoneid).arg(m_currentchannel)); // Call
            }
        } else if (action == "dtransfer") {
            dtransfer();
            if (linestatus == WDTransfer || linestatus == WITransfer)
                updateLine(m_currentchannel, (QStringList() << "hangup" << "dtransfer" << "itransfer"));
            else if (linestatus == Ringing)
                updateLine(m_currentchannel, (QStringList() << "hangup" << "dtransfer"));
            else
                updateLine(m_currentchannel, (QStringList() << "hangup" << "dtransfer" << "itransfer" << "numreturn"));
        } else if (action == "itransfer") {
            itransfer();
            if (linestatus == WDTransfer || linestatus == WITransfer)
                updateLine(m_currentchannel, (QStringList() << "hangup" << "dtransfer" << "itransfer"));
            else
                updateLine(m_currentchannel, (QStringList() << "hangup" << "dtransfer" << "itransfer" << "numreturn"));
        } else if (action == "park") {
            b_engine->actionCall("transfer", "chan:special:me:" + m_currentchannel, "ext:special:parkthecall");
        } else if (action == "atxferfinalize") {
            b_engine->actionCall("hangup", QString("chan:%1:%2").arg(m_xphoneid).arg(m_currentchannel));
        } else if (action == "atxfercancel") {
            b_engine->actionCall("hangup", QString("chan:%1:%2").arg(m_xphoneid).arg(getPeerChan(m_currentchannel)));
            updateLine(m_currentchannel, (QStringList() << "hangup" << "dtransfer" << "itransfer" << "park"));
        } else if (action == "ilink") {
            b_engine->actionCall("hangup", QString("chan:%1:%2").arg(m_xphoneid).arg(m_currentchannel));
        } else if (action == "icancel") {
            // the CTI server will find the appropriate related channel to hangup
            b_engine->actionCall("transfercancel", QString("chan:%1:%2").arg(m_xphoneid).arg(m_currentchannel));
            // emit actionCall("hangup", QString("chan:%1:%2").arg(m_xphoneid).arg(getPeerChan(m_currentchannel)));  // does nothing
            // emit actionCall("hangup", QString("chan:%1:%2").arg(m_xphoneid).arg(m_currentchannel)); // finalize the indirect transfer
        }
        //            if (action == "unpark")
        //                qDebug() << Q_FUNC_INFO << "F1 when Wait : Take back";
    }
}

/*! \brief set lines width according to current channel
 */
void XletOperator::changeCurrentChannel(const QString & before, const QString & after)
{
    // qDebug() << Q_FUNC_INFO << before << after;
    if (before != after) {
        if (m_vlinesl.contains(before) && m_vlinesr.contains(before)) {
            m_vlinesl[before]->setLineWidth(1);
            m_vlinesr[before]->setLineWidth(1);
        }
        if (m_vlinesl.contains(after) && m_vlinesr.contains(after)) {
            m_vlinesl[after]->setLineWidth(3);
            m_vlinesr[after]->setLineWidth(3);
        }
    }
}

void XletOperator::updatePhoneConfig(const QString & xphoneid)
{
}

void XletOperator::updatePhoneStatus(const QString & xphoneid)
{
    if (xphoneid != m_xphoneid)
        return;
    const PhoneInfo * phoneinfo = b_engine->phone(xphoneid);
    if (phoneinfo == NULL)
        return;

    foreach (const QString channel, phoneinfo->channels()) {
        const ChannelInfo * channelinfo = b_engine->channels().value(channel);
        if (channelinfo == NULL)
            continue;
        const QString status = channelinfo->commstatus();
        const QString todisplay = channelinfo->peerdisplay();
        if (status == CHAN_STATUS_RINGING) {
            if (!m_callchannels.contains(channel)) {
                newCall(channel);
                m_callchannels << channel;
                m_linestatuses[channel] = Ringing;
                QStringList action = QStringList() << "hangup"
                                                   << "dtransfer"
                                                   << "park";
                if (b_engine->getGuiOptions("client_gui").value("xlet_operator_answer_work", 1).toInt()) {
                    action << "answer";
                }
                updateLine(channel, action);
                m_statuses[channel]->setText(tr("%1 Ringing").arg(todisplay));
                m_statuses[channel]->show();
            }
        } else if ((status == CHAN_STATUS_LINKED_CALLED) ||
                   (status == CHAN_STATUS_LINKED_CALLER)) {
            if (!m_callchannels.contains(channel)) {
                newCall(channel);
                m_callchannels << channel;
            }
            m_linestatuses[channel] = Online;
            QStringList allowed;
            allowed << "hangup" << "dtransfer" << "itransfer" << "park";
            // if (qvm["atxfer"].toBool()) XXXX
            // allowed << "atxferfinalize" << "atxfercancel";
            updateLine(channel, allowed);
            m_statuses[channel]->setText(tr("Link %1").arg(todisplay));
            m_statuses[channel]->show();
        } else if (status == CHAN_STATUS_HANGUP) {
            if (m_callchannels.contains(channel)) {
                removeLine(channel);
            }
        } else {
            qDebug() << Q_FUNC_INFO << "not processed" << channel << status;
        }
    }

    foreach (const QString channel, m_callchannels) {
        if (! phoneinfo->channels().contains(channel)) {
            removeLine(channel);
        }
    }
}

void XletOperator::updateChannelStatus(const QString &)
{
    // qDebug() << Q_FUNC_INFO << xchannelid;
}

void XletOperator::updateUserConfig(const QString & xuserid)
{
    m_ui = b_engine->getXivoClientUser();
    m_xuserid = m_ui->xid();
    m_xphoneid = m_ui->phonelist().join("");

    if (! m_ui)
        return;
    if (xuserid != m_xuserid)
        return;
    // qDebug() << Q_FUNC_INFO << xuserid;
    m_lbl->setText(m_ui->fullname());
}

void XletOperator::updateUserStatus(const QString & xuserid)
{
    if (! m_ui)
        return;
    if (xuserid != m_xuserid)
        return;
    // qDebug() << Q_FUNC_INFO << xuserid;
}

/*! \brief get the peer channel linked to channel
 *
 * Iterate through communications of the user to
 * find the peer channel.
 */
QString XletOperator::getPeerChan(QString const &) const
{
//     if (! m_ui)
//         return QString();
//     QString ipbxid = m_ui->ipbxid();
//     foreach (const QString phoneid, m_ui->phonelist()) {
//         QString xphoneid = QString("%1/%2").arg(ipbxid).arg(phoneid);
//         const PhoneInfo * pi = b_engine->phone(xphoneid);
//         if (pi == NULL)
//             continue;
//         foreach (const QString channel, pi->channels()) {
//             const ChannelInfo * channelinfo = b_engine->channels().value(channel);
//             if (channelinfo == NULL)
//                 continue;

//             QMapIterator<QString, QVariant> it(pi->comms());
//             while (it.hasNext()) {
//                 it.next();
//                 QVariantMap qvm = it.value().toMap();
//                 if (qvm["thischannel"].toString() == chan) {
//                     QString peerchan = qvm["peerchannel"].toString();
//                     // ugly workaround...
//                     if (peerchan.endsWith("<MASQ>"))
//                         peerchan.remove("<MASQ>");
//                     return peerchan;
//                 }
//             }
//         }
//     }
    return QString();
}

/*! \brief remove widgets when calls are finished
 *
 * clean everything.
 */
void XletOperator::removeLine(QString const & chan)
{
    m_vlinesl.take(chan)->deleteLater();
    m_vlinesr.take(chan)->deleteLater();
    m_statuses.take(chan)->deleteLater();
    m_tnums.take(chan)->deleteLater();
    m_linestatuses.remove(chan);
    QHashIterator<QString, QPushButton *> it(m_actions[chan]);
    while (it.hasNext()) {
        it.next();
        it.value()->deleteLater();
    }
    m_actions.remove(chan);
    // current channel is next channel
    if (m_currentchannel == chan) {
        if (m_callchannels.size() > 1) {
            int ci = m_callchannels.indexOf(m_currentchannel);
            if (ci == m_callchannels.size() - 1)
                ci --;
            changeCurrentChannel(m_currentchannel, m_callchannels[ci]);
            m_currentchannel = m_callchannels[ci];
        } else
            m_currentchannel = "";
    }
    m_callchannels.removeAll(chan);
    m_row.remove(chan);
}

void XletOperator::doGUIConnects(QWidget *mainwindow)
{
    connect(mainwindow, SIGNAL(functionKeyPressed(int)),
            this, SLOT(functionKeyPressed(int)));
}
