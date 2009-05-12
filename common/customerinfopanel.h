/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
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
#include <QWidget>

#include "popup.h"
#include "xletprotopanel.h"

class QTabWidget;

class Popup;
class BaseEngine;

/*! \brief display "sheets" from calling customers
 */
class CustomerInfoPanel : public XletprotoPanel
{
    Q_OBJECT
public:
    CustomerInfoPanel(BaseEngine *,
                      const QVariant &,
                      QWidget * parent = 0);
    ~CustomerInfoPanel();
signals:
    void newPopup(const QString &,
                  const QHash<QString, QString> &,
                  const QString &);
    void actionFromFiche(const QVariant &);
    void actionCall(const QString &, const QString &, const QString &);
public slots:
    void showNewProfile(Popup *);
    void popupDestroyed(QObject * obj);
    void addToDataBase(const QString &);
    void displayFiche(const QString &, bool, const QString &);
    void actionFromPopup(const QString &, const QVariant &);
    void localActionCall(const QString &, const QString &, const QString &);
    void activateRemarkArea(const QString & id);
    void desactivateRemarkArea(const QString & id);
    void addNewRemark(const QString & id, const QVariantMap & entry);
private:
    QTabWidget * m_tabs;
    QList<Popup *> m_popups;
    quint32 m_tablimit;
    bool m_autourl_allowed;
};

#endif /* __CUSTOMERINFOPANEL_H__ */
