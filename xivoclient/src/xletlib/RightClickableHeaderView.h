/*
 * RightClickableHeaderView.h
 *
 *  Created on: 2013-06-11
 *      Author: pcm
 */

#ifndef RIGHTCLICKABLEHEADERVIEW_H_
#define RIGHTCLICKABLEHEADERVIEW_H_

#include <qheaderview.h>

class RightClickableHeaderView: public QHeaderView {
public:
    RightClickableHeaderView(Qt::Orientation orientation, QWidget *parent);
    ~RightClickableHeaderView();
protected:
    virtual void contextMenuEvent(QContextMenuEvent *);
};

#endif /* RIGHTCLICKABLEHEADERVIEW_H_ */
