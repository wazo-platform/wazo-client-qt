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

#ifndef __VIDEOPLAYERWIDGET_H__
#define __VIDEOPLAYERWIDGET_H__

#include <QApplication>
#include <QProcess>
#include <QVBoxLayout>
#include <QLayoutItem>
#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QColor>
#include <QRect>
#include <QLinearGradient>
#include <QSizePolicy>
#include <QPushButton>
#include <QTextEdit>
#include <QSlider>
#include <QCloseEvent>
#include <QTimer>
#include "xlet.h"

/*! \brief to be documented !
 */
class PlayerWidget: public XLet
{
Q_OBJECT
    public:
    PlayerWidget(BaseEngine * engine, QWidget * parent = 0);
protected:
    virtual void closeEvent(QCloseEvent *);
private:
    bool startMPlayer();
    bool stopMPlayer();

    private slots:
    void catchOutput();
    void pollCurrentTime();
    // Dirige la timeline
    void timeLineChanged(int);
    // Play/stop
    void switchPlayState();
    void mplayerEnded(int, QProcess::ExitStatus);
private:
    QPushButton *controller;
    QWidget *renderTarget;
    QProcess *mplayerProcess;
    bool isPlaying;
    QSlider *timeLine;
    QTimer *poller;
    QTextEdit *log;
    QString m_movie_url;
};

#endif /* __VIDEOPLAYERWIDGET_H__ */
