#ifndef _CONFERENCE2_CONFCHAMBER_H_
#define _CONFERENCE2_CONFCHAMBER_H_
#include <QWidget>
#include <QDebug>

class ConfChamber : public QWidget
{
    Q_OBJECT

    public:
        ConfChamber(const QString &id);
};

#endif
