#ifndef _CURRENTCALLWIDGET_H_
#define _CURRENTCALLWIDGET_H_

#include "pendingwidget.h"

class CurrentCallWidget: public PendingWidget
{

Q_OBJECT

public:
    CurrentCallWidget(const QString &, const QString &, int, QWidget *);
    ~CurrentCallWidget();
    void update();
    bool toRemove() const;
    int line() const;
    const QString & channelid() const;
public slots:
    void doPickup() { }; // Do nothing
    void doAtxfer();
    void doConf();
    void doHangup();
    void doHold();
    void doPark();
    void doTxfer();
signals:
    void atxfer();
    // void blindTransfer(const QString &, int, const QString &, const QString &);
    void hangup();
    void hold();
    void park();
    void selectLine(int);
    void remove_me();
    void txfer();
protected:
    void buildui();
private:
    int m_line; //!< The phone's line number
    QString m_chan_xid;
    QPushButton * m_btn_atxfer;
    QPushButton * m_btn_txfer;
    QPushButton * m_btn_hold;
    QPushButton * m_btn_hangup;
    QPushButton * m_btn_conf;
    QPushButton * m_btn_park;
};

inline int CurrentCallWidget::line() const
{
    return m_line;
}

inline const QString & CurrentCallWidget::channelid() const
{
    return m_chan_xid;
}

#endif /* _CURRENTCALLWIDGET_H_ */
