/* XiVO Client
 * Copyright (C) 2007-2013, Avencall
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

#ifndef __CUSTOMERINFOPANEL_H__
#define __CUSTOMERINFOPANEL_H__

#include <QHash>
#include <QList>
#include <QObject>
#include <QBuffer>
#include <QDateTime>
#include <QDebug>
#include <QLabel>
#include <QGridLayout>
#include <QTabWidget>
#include <QVariant>

#include <xlet.h>
#include <popup.h>
#include <baseengine.h>
#include "functests.h"


class Popup;

/*! \brief display "sheets" from calling customers
 */
class CustomerInfoPanel : public XLet
{
    Q_OBJECT

    public:
        CustomerInfoPanel(QWidget *parent=0);

        void doGUIConnects(QWidget *mainwindow);

    signals:
        void newPopup(const QString &, const QHash<QString, QString> &, const QString &);
        void actionFromFiche(const QVariant &);
        void showWidgetOnTop(QWidget *);

    public slots:
        void showNewProfile(Popup *);
        void popupDestroyed(QObject *obj);
        void displayFiche(const QString &, bool, const QString &);
        void actionFromPopup(const QString &, const QVariant &);
        void activateRemarkArea(const QString &id);
        void desactivateRemarkArea(const QString &id);
        void addNewRemark(const QString &id, const QVariantMap &entry);

    private:
        QGridLayout * m_glayout;
        QTabWidget * m_tabs;
        QList<Popup *> m_popups;
        quint32 m_tablimit;
        bool m_autourl_allowed;

    FUNCTESTED
};

#endif /* __CUSTOMERINFOPANEL_H__ */
