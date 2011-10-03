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

#ifndef __DIRDIALOG_H__
#define __DIRDIALOG_H__

#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

#include <baseengine.h>
#include <directorypanel.h>

/*! \brief Directory dialog used for fax
 */
class DirDialog: public QDialog
{
    Q_OBJECT

    public:
        /*! \brief Constructor
         *
         * Construct the widget and its layout.
         * Fill widgets with values got from the BaseEngine object.
         * Once constructed, the Widget is ready to be shown.
         * \param engine        related BaseEngine object where parameters will be modified
         * \param parent        parent QWidget
         */
        DirDialog(QWidget *);
        ~DirDialog();
        const QString & faxnumber() const;

    private slots:
        void saveAndClose();
        void copyNumber(const QString &);
        void copyNumberAndQuit(const QString &, bool);

    private:
        DirectoryPanel *m_directory;   //!< Directory panel
        QDialogButtonBox *m_btnbox;    //!< Buttons box
        QString m_faxnumber;            //!< fax number
        QString m_retfaxnumber;         //!< fax number to return
};
#endif
