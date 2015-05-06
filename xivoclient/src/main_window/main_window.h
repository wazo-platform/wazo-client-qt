/* XiVO Client
 * Copyright (C) 2013-2015 Avencall
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

#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QList>
#include <QMainWindow>
#include <QSize>
#include <QtWidgets>

#include <ui_main_window.h>
#include <xletlib/functests.h>
#include <config_widget/config_widget.h>


class ConfigWidget;
class QCloseEvent;
class QResizeEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    FUNCTESTED

    public:
        MainWindow(QWidget *parent = 0);
        ~MainWindow();
        void initialize();
        void saveDefaultState();
        void saveStateToConfigFile();
        void restoreStateFromConfigFile();
        bool isFolded();
        void setFolded(const QSize size);
        void restoreFolded();

        Ui::MainWindow *ui;

    public slots:
        void restoreDefaultState();
        void setAppIcon(const QString & def);
        void showWindow();
        void hideWindow();
        void customerInfoPopup(const QString &, const QHash<QString, QString> &, const QString &);
        void prepareState();

    private slots:
        void clipselection();
        void clipdata();
        void showMessageBox(const QString &);
        void showConfDialog();
        void cleanConfDialog();
        void confUpdated();
        void setStatusNotLogged();
        void setStatusLogged();
        void minimizeWindow();
        void about();
        void showCredits();

    protected:
        virtual void closeEvent(QCloseEvent *);
        virtual void resizeEvent(QResizeEvent *event);

    signals:
        void initialized();
        void titleUpdated(const QString & title);
        void iconUpdated(const QIcon & icon);

    private:
        void setTitle(const QString &);

        ConfigWidget *m_config_widget;
        QClipboard * m_clipboard;

        QByteArray m_default_state;
        QDateTime m_launch_date_time;

        bool m_folded;
        int m_minimum_height;
        QSize m_unfolded_size;
};

#endif
