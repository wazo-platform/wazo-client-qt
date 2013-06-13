/*
 * RightClickableHeaderView.h
 *
 *  Created on: 2013-06-11
 *      Author: pcm
 */

#ifndef RIGHTCLICKABLEHEADERVIEW_H_
#define RIGHTCLICKABLEHEADERVIEW_H_

#include <qheaderview.h>
#include <QList>
#include <QPair>

class RightClickableHeaderView: public QHeaderView {

    Q_OBJECT

    public:
        RightClickableHeaderView(Qt::Orientation orientation, QWidget *parent);
        ~RightClickableHeaderView();
    signals:
        void headerContextMenuEvent(QContextMenuEvent *e);
    protected:
        virtual void contextMenuEvent(QContextMenuEvent *e);
};

#endif /* RIGHTCLICKABLEHEADERVIEW_H_ */
