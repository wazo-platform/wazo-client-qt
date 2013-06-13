/*
 * RightClickableHeaderView.cpp
 *
 *  Created on: 2013-06-11
 *      Author: pcm
 */

#include "RightClickableHeaderView.h"

#include <QDebug>
#include <QContextMenuEvent>
#include <QAbstractItemModel>
#include <QTableView>
#include <QMenu>
#include <QAction>

RightClickableHeaderView::RightClickableHeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{
}

RightClickableHeaderView::~RightClickableHeaderView()
{
}

void RightClickableHeaderView::contextMenuEvent(QContextMenuEvent *e)
{
    emit headerContextMenuEvent(e);
}
