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

#include <QApplication>
#include <QDebug>
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

#include "videopanel.h"

#ifdef Q_OS_WIN32
const QString mplayerPath("path/to/mplayer.exe");
#else
const QString mplayerPath("mplayer");
#endif

PlayerWidget::PlayerWidget(QWidget * parent)
    : XLet(parent),
    isPlaying(false),
    m_movie_url("")
{
    setTitle( tr("Video") );
    controller = new QPushButton("Play");
        
    renderTarget = new QWidget(this);
    renderTarget->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    renderTarget->setAttribute(Qt::WA_PaintOnScreen);
    renderTarget->setMinimumSize(176, 144);
        
    timeLine = new QSlider(Qt::Horizontal);
        
    //log = new QTextEdit;
    //log->setReadOnly(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(controller);
    layout->addWidget(renderTarget, 1, Qt::AlignHCenter);
    layout->addWidget(timeLine);
    //layout->addWidget(log);
    setLayout(layout);

    mplayerProcess = new QProcess(this);

    poller = new QTimer(this);

    connect(controller, SIGNAL(clicked()), this, SLOT(switchPlayState()));
    connect(mplayerProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(catchOutput()));
    connect(mplayerProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(mplayerEnded(int, QProcess::ExitStatus)));
    connect(poller, SIGNAL(timeout()), this, SLOT(pollCurrentTime()));
    connect(timeLine, SIGNAL(sliderMoved(int)), this, SLOT(timeLineChanged(int)));
}

void PlayerWidget::closeEvent(QCloseEvent *e)
{
    stopMPlayer();
    e->accept();
}

bool PlayerWidget::startMPlayer()
{
    if(isPlaying)
        return true;

    QStringList args;
        
    // On demande à utiliser mplayer comme backend
    args << "-slave";
    // Et on veut ne pas avoir trop de chose à parser :)
    args << "-quiet";
#ifdef Q_WS_WIN
    // reinterpret_cast<qlonglong> obligatoire, winId() ne se laissant pas convertir gentiment ;)
    args << "-wid" << QString::number(reinterpret_cast<qlonglong>(renderTarget->winId()));
    args << "-vo" << "directx:noaccel";
#else
    // Sur linux, aucun driver n'a été nécessaire et pas de manip pour Wid :)
    // args << "-vf" << "dsize=300:200:0";
    args << "-wid" << QString::number(renderTarget->winId());

    /*
      log->append("Video output driver may not be necessary for your platform. \
      Check: http://www.mplayerhq.hu/DOCS/man/en/mplayer.1.html     \
      at the VIDEO OUTPUT DRIVERS section.");
    */
#endif
    args << m_movie_url;

    // On parse la stdout et stderr au même endroit, donc on demande à "fusionnner" les 2 flux
    mplayerProcess->setProcessChannelMode(QProcess::MergedChannels);
    mplayerProcess->start(mplayerPath, args);
    if(!mplayerProcess->waitForStarted(3000)) {
        qDebug("allez, cherche le bug :o");
        return false;
    }

    // On récupère les infos de base
    mplayerProcess->write("get_video_resolution\n");
    mplayerProcess->write("get_time_length\n");

    poller->start(1000);

    isPlaying = true;

    return true;
}

bool PlayerWidget::stopMPlayer()
{
    if(!isPlaying)
        return true;

    mplayerProcess->write("quit\n");
    renderTarget->setMinimumSize(176, 144);
    if(!mplayerProcess->waitForFinished(3000)) {
        qDebug("ZOMG, ça plante :(");
        return false;
    }

    return true;
}

void PlayerWidget::catchOutput()
{
    while(mplayerProcess->canReadLine()) {
        QByteArray buffer(mplayerProcess->readLine());
        //log->append(QString(buffer));
                        
        // On vérifie si on a eu des réponses
        if(buffer.startsWith("ANS_VIDEO_RESOLUTION")) {
            // réponse à get_video_resolution : ANS_VIDEO_RESOLUTION='<width> x <height>'
            buffer.remove(0, 21); // vire ANS_VIDEO_RESOLUTION=
            buffer.replace(QByteArray("'"), QByteArray(""));
            buffer.replace(QByteArray(" "), QByteArray(""));
            buffer.replace(QByteArray("\n"), QByteArray(""));
            buffer.replace(QByteArray("\r"), QByteArray(""));
            int sepIndex = buffer.indexOf('x');
            int resX = buffer.left(sepIndex).toInt();
            int resY = buffer.mid(sepIndex+1).toInt();
            renderTarget->setMinimumSize(resX, resY);
        } else if(buffer.startsWith("ANS_LENGTH")) {
            // réponse à get_time_length : ANS_LENGTH=xx.yy
            buffer.remove(0, 11); // vire ANS_LENGTH=
            buffer.replace(QByteArray("'"), QByteArray(""));
            buffer.replace(QByteArray(" "), QByteArray(""));
            buffer.replace(QByteArray("\n"), QByteArray(""));
            buffer.replace(QByteArray("\r"), QByteArray(""));
            float maxTime = buffer.toFloat();
            timeLine->setMaximum(static_cast<int>(maxTime+1));
        } else if(buffer.startsWith("ANS_TIME_POSITION")) {
            // réponse à get_time_pos : ANS_TIME_POSITION=xx.y
            buffer.remove(0, 18); // vire ANS_TIME_POSITION=
            buffer.replace(QByteArray("'"), QByteArray(""));
            buffer.replace(QByteArray(" "), QByteArray(""));
            buffer.replace(QByteArray("\n"), QByteArray(""));
            buffer.replace(QByteArray("\r"), QByteArray(""));
            float currTime = buffer.toFloat();
            timeLine->setValue(static_cast<int>(currTime+1));
        }
    }
}

void PlayerWidget::pollCurrentTime()
{
    mplayerProcess->write("get_time_pos\n");
}

// Dirige la timeline
void PlayerWidget::timeLineChanged(int pos)
{
    mplayerProcess->write(QString("seek " + QString::number(pos) + " 2\n").toUtf8());
}

// Play/stop
void PlayerWidget::switchPlayState()
{
    if(!isPlaying) {
        if(!startMPlayer())
            return;
                
        // log->clear();
        controller->setText("Stop");
        isPlaying = true;
    } else {
        if(!stopMPlayer())
            return;

        poller->stop();
        // log->clear();
        controller->setText("Play");
        isPlaying = false;
    }
}

void PlayerWidget::mplayerEnded(int, QProcess::ExitStatus)
{
    isPlaying = false;
    controller->setText("Play");
    poller->stop();
}
