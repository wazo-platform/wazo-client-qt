/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

#ifndef __FAXPANEL_H__
#define __FAXPANEL_H__

#include <QDebug>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>
#include <QLineEdit>

#include <xlet.h>
#include <baseengine.h>
#include <dirdialog.h>
#include <extendedlineedit.h>

/*! \brief Panel used to send an receive fax
 */
class FaxPanel : public XLet
{
    Q_OBJECT

    public:
        FaxPanel(QWidget *parent=0);
        ~FaxPanel();

    signals:
        void faxSend(const QString &, const QString &, Qt::CheckState);

    public slots:
        void setOpenFileName();
        void sendFax();
        void dirLookup();
        void popupMsg(const QString &, const QString &);
        void destNumberChanged(const QString &);
        void destSelectionChanged();
        void fileNameChanged(const QString &);

    private:
        QWidget *m_mainwindow;  //!< MainWidget where some parameters are commited to
        FileNameLineEdit *m_openFileNameLabel;
        QLineEdit *m_destination;
        QCheckBox *m_maskornot;
        QPushButton *m_sendButton;
        DirDialog *m_dirw;

        QString m_dest_string;
        QString m_file_string;
};

#endif
