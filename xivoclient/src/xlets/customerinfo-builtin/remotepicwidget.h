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

#ifndef __REMOTEPICWIDGET_H__
#define __REMOTEPICWIDGET_H__

#include <QWidget>
#include <QHttp>
#include <QFile>
#include <QLabel>
#include <QSslError>
#include <QTemporaryFile>

//! Downloads and displays a picture
class RemotePicWidget : public QWidget
{
    Q_OBJECT

    public:
        RemotePicWidget(const QString &name, const QString &url, QWidget *parent=0);
        void startHttpRequest(const QString &);  //! Starts the download process
    private slots:
        void httpRequestFinished(int, bool);  //! connected to the requestFinished() signal
        void httpDataReadProgress(int, int);  //! connected to the dataReadProgress() signal
        void httpReadResponseHeader(const QHttpResponseHeader &);  //! connected to the responseHeaderReceived() signal
        void httpSslErrors(const QList<QSslError> &);

    private:
        QHttp *m_http;  //!< QHttp object
        QFile *m_tempFile;  //!< Temporary file for storing the picture
        QLabel *m_label;  //!< QLabel widget used to display the picture
        int m_httpGetId;
};

#endif
