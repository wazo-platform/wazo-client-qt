/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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


#include "customerinfopanel.h"

/*! \brief Constructor
 */
CustomerInfoPanel::CustomerInfoPanel(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Sheets"));
    connect(b_engine, SIGNAL(displayFiche(const QString &, bool, const QString &)),
            this, SLOT(displayFiche(const QString &, bool, const QString &)));
    connect(b_engine, SIGNAL(gotSheetOwnership(const QString &)),
            this, SLOT(activateRemarkArea(const QString &)));
    connect(b_engine, SIGNAL(lostSheetOwnership(const QString &)),
            this, SLOT(desactivateRemarkArea(const QString &)));
    connect(b_engine, SIGNAL(sheetEntryAdded(const QString &, const QVariantMap &)),
            this, SLOT(addNewRemark(const QString &, const QVariantMap &)));
    connect(this, SIGNAL(actionFromFiche(const QVariant &)),
            b_engine, SLOT(actionFromFiche(const QVariant &)));

    // qDebug() << Q_FUNC_INFO;
    QGridLayout * glayout = new QGridLayout(this);
    m_tabs = new QTabWidget(this);
    glayout->addWidget( m_tabs, 0, 0 );
    glayout->setRowStretch(0, 1);
    glayout->setColumnStretch(0, 1);
    QVariantMap optionsMap = b_engine->getGuiOptions("merged_gui");
    m_tablimit = optionsMap["sheet-tablimit"].toUInt();
    m_autourl_allowed = optionsMap["autourl_allowed"].toBool();
}

/*!
 * Display the new profile in the tabbed area
 * and show a message with the systray icon
 */
void CustomerInfoPanel::showNewProfile(Popup * popup)
{
    QString opt = "";
    qDebug() << Q_FUNC_INFO
             << popup->callKind()
             << popup->callAstid() << popup->callContext()
             << popup->callUniqueid() << popup->callChannel();
    if(popup->sheetpopup()) {
        Popup *already_popup = NULL;
        foreach(Popup *mpopup, m_popups)
            if ( (mpopup->callAstid() == popup->callAstid()) &&
                 (mpopup->callContext() == popup->callContext()) &&
                 (mpopup->callUniqueid() == popup->callUniqueid()) ) {
                already_popup = mpopup;
                break;
            }
        if(already_popup) {
            qDebug() << Q_FUNC_INFO << "found a match for"
                     << popup->callAstid() << popup->callContext() << popup->callUniqueid();
            already_popup->update(popup->sheetlines());
        } else {
            QString currentTimeStr = QDateTime::currentDateTime().toString("hh:mm:ss");
            quint32 index = m_tabs->addTab(popup, currentTimeStr);
            qDebug() << Q_FUNC_INFO << "added tab" << index;
            m_popups.append(popup);
            m_tabs->setCurrentIndex(index);
            if (index >= m_tablimit)
                // close the first widget
                m_tabs->removeTab(0);
        }
        
        // no need to focus if there is no sheet popup
        if(popup->focus())
            opt += "fp";
    }
    
    // tells the main widget that a new popup has arrived here
    if(popup->systraypopup())
        opt += "s";
    newPopup(popup->messagetitle(), popup->message(), opt);
    // set this widget to be the current tab in XiVO Client
    emit showWidgetOnTop(this);
}

void CustomerInfoPanel::popupDestroyed(QObject * obj)
{
    qDebug() << Q_FUNC_INFO
             << obj->property("astid") << obj->property("uniqueid") << obj->property("context");
    foreach(Popup * mpopup, m_popups)
        if ( (mpopup->callAstid() == obj->property("astid").toString()) &&
             (mpopup->callContext() == obj->property("context").toString()) &&
             (mpopup->callUniqueid() == obj->property("uniqueid").toString()) )
            m_popups.removeAll(mpopup);
}

void CustomerInfoPanel::displayFiche(const QString & fichecontent, bool qtui, const QString & id)
{
    qDebug() << Q_FUNC_INFO << id;
    for(int i = m_popups.size() - 1; --i > 0; ) {
        if(id == m_popups[i]->id()) {
            qDebug() << " fiche id already there";
        }
    }
    QBuffer *inputstream = new QBuffer(this);
    inputstream->open(QIODevice::ReadWrite);
    inputstream->write(fichecontent.toUtf8());
    inputstream->close();
    // Get Data and Popup the profile if ok
    Popup *popup = new Popup(m_autourl_allowed);
    popup->setId(id);
    connect(popup, SIGNAL(destroyed(QObject *)),
            this, SLOT(popupDestroyed(QObject *)));
    connect(popup, SIGNAL(wantsToBeShown(Popup *)),
            this, SLOT(showNewProfile(Popup *)));
    connect(popup, SIGNAL(actionFromPopup(const QString &, const QVariant &)),
            this, SLOT(actionFromPopup(const QString &, const QVariant &)));
    connect(popup, SIGNAL(newRemarkSubmitted(const QString &, const QString &)),
            b_engine, SLOT(sendNewRemark(const QString &, const QString &)));
    popup->feed(inputstream, qtui);
}

void CustomerInfoPanel::actionFromPopup(const QString & buttonname, const QVariant & timestamps)
{
    QString astid = sender()->property("astid").toString();
    QString uniqueid = sender()->property("uniqueid").toString();
    QString context = sender()->property("context").toString();
    Popup *thispopup = NULL;
    foreach(Popup *mpopup, m_popups)
        if ( (mpopup->callUniqueid() == uniqueid) &&
             (mpopup->callContext() == context) &&
             (mpopup->callAstid() == astid) ) {
            thispopup = mpopup;
            break;
        }
    if(thispopup) {
        QVariantMap data;
        data["buttonname"] = buttonname;
        data["astid"] = thispopup->callAstid();
        data["uniqueid"] = thispopup->callUniqueid();
        data["context"] = thispopup->callContext();
        data["channel"] = thispopup->callChannel();
        data["timestamps"] = timestamps;
        actionFromFiche(QVariant(data));
    }
}

void CustomerInfoPanel::activateRemarkArea(const QString & id)
{
    foreach(Popup *mpopup, m_popups) {
        if(mpopup->id() == id)
            mpopup->activateRemarkArea();
    }
}

void CustomerInfoPanel::desactivateRemarkArea(const QString & id)
{
    foreach(Popup *mpopup, m_popups) {
        if(mpopup->id() == id)
            mpopup->desactivateRemarkArea();
    }
}

void CustomerInfoPanel::addNewRemark(const QString & id, const QVariantMap & entry)
{
    qDebug() << Q_FUNC_INFO << id << entry;
    foreach(Popup *mpopup, m_popups) {
        if(mpopup->id() == id)
            mpopup->addRemark(entry);
    }
}

void CustomerInfoPanel::doGUIConnects(QWidget * mainwindow)
{
    connect(this, SIGNAL(newPopup(const QString &, const QHash<QString, QString> &, const QString &)),
            mainwindow, SLOT(customerInfoPopup(const QString &, const QHash<QString, QString> &, const QString &)));
    connect(this, SIGNAL(showWidgetOnTop(QWidget *)),
            mainwindow, SLOT(showWidgetOnTop(QWidget *)));
}
