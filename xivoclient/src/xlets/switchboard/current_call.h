/* XiVO Client
 * Copyright (C) 2012, Avencall
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

#ifndef __CURRENT_CALL_H__
#define __CURRENT_CALL_H__

#include <ipbxlistener.h>
#include <QObject>

class QWidget;
namespace Ui {
    class CurrentCallWidget;
}

class CurrentCall: public QObject, public IPBXListener
{
    Q_OBJECT

    public:
        CurrentCall(QObject *parent=NULL);
        ~CurrentCall();
        void setParentWidget(QWidget *parent);
        virtual void parseCommand(const QVariantMap &command);
    public slots:
        void hangup();
        void hold();
    private slots:
        void updateCallInfo();
    private:
        void clear();
        void updateCallerID(const QString &name, const QString &number);
        void updateCall(const QVariantList &calls);
        void enableButtons();
        void disableButtons();
        void setButtonsAvailability(bool enabled);
        void connectButtons();

        Ui::CurrentCallWidget *m_current_call_widget;
        double m_call_start;
        QString m_caller_id;
};

#endif /* __CURRENT_CALL_H__ */
