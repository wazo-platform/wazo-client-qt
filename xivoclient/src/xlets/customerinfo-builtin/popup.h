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

#ifndef __POPUP_H__
#define __POPUP_H__

#include <QHash>
#include <QVariant>
#include <QWidget>
#include "xmlhandler.h"

class QBuffer;
class QFrame;
class QLabel;
class QLineEdit;
class QPushButton;
class QUrl;
class QXmlInputSource;
class QXmlSimpleReader;
class QIODevice;
class QVBoxLayout;
class QHBoxLayout;
class QUiLoader;

class UserInfo;
class RemarkArea;
class BaseEngine;

class Popup: public QWidget
{
    Q_OBJECT

    public:
        Popup(const bool , QWidget *parent=0);
        ~Popup();
        void feed(QIODevice *, const bool &);
        void addInfoInternal(const QString &, const QString &);  //! Add a Text field (name, value)
        void addInfoText(int, const QString &, const QString &);  //! Add a url field
        void addInfoLink(int, const QString &, const QString &);
        void addInfoLinkX(int, const QString &, const QString &);
        void addInfoPicture(int, const QString &, const QString &);
        void addInfoPhone(int, const QString &, const QString &);
        void setMessage(const QString &, const QString &);
        const QHash<QString, QString>& message() const;
        void setMessageTitle(const QString &);
        const QString& messagetitle() const;
        void finishAndShow();
        void setSheetPopup(const bool &);
        bool sheetpopup();
        const QString& callIpbxId() const;
        const QString& callChannel() const;
        const QString& callWhere() const;
        bool systraypopup();
        bool focus();
        void setTitle(const QString &);

        void addAnyInfo(const QString &, const QString &, const QString &, const QString &, const QString &);
        void addDefForm(const QString &, const QString &);
        void update(QList<QStringList> &);
        QList<QStringList>& sheetlines();
        void addRemarkArea();
        void activateRemarkArea();
        void desactivateRemarkArea();
        void addRemark(const QVariantMap &entry);
        const QString& id() const { return m_id; };
        void setId(const QString &id) { m_id = id; };

    signals:
        void wantsToBeShown(Popup *);
        void actionFromPopup(const QString &, const QVariant &);
        void newRemarkSubmitted(const QString &, const QString &);

    public slots:
        void streamNewData();
        void streamAboutToClose();
        void socketDisconnected();
        void dialThisNumber();
        void dispurl(const QUrl &);
        void httpGetNoreply();
        void actionFromForm();
        void newRemark(const QString &);

    protected:
        void closeEvent(QCloseEvent *);

    private:
        void addInfoForm(int, const QString &);
        void saveandclose();
        void setEnablesOnForms();

        QIODevice * m_inputstream;  //!< input stream where the XML is read from
        QBuffer * m_buffer;  //!< buffer where the XML is read from

        QXmlInputSource * m_xmlInputSource; //! XML parser object.
        QXmlSimpleReader * m_reader;
        XmlHandler * m_handler;
        bool m_parsingStarted;
        QVBoxLayout * m_vlayout;
        QHash<QString, QString> m_message;
        QString m_messagetitle;
        QLabel * m_title;
        QPushButton * m_closesheet;
        QHBoxLayout * m_hlayout;
        QFrame * m_qf;

        QString m_ipbxid;
        QString m_channel;
        QString m_where;

        bool m_sheetpopup;
        bool m_systraypopup;
        bool m_focus;
        bool m_urlautoallow;
        bool m_toupdate;
        bool m_sheetui;
        int m_firstline;
        QWidget * m_sheetui_widget;
        QUiLoader * m_uiloader;
        QStringList m_orders;
        QList<QStringList> m_sheetlines;
        QHash<QString, QPushButton *> m_form_buttons;
        QHash<QString, QString> m_remoteforms;
        QVariantMap m_timestamps;
        RemarkArea * m_remarkarea;  //!< user editable area
        int m_nfeeds;
        QString m_id;
};

#endif
