#include "pendingwidget.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include <limits>

#include "baseengine.h"

unsigned int PendingWidget::counted = 0;

PendingWidget::PendingWidget(const QString & phonexid, QWidget * parent)
    :QWidget(parent), m_id(PendingWidget::counted++), m_phonexid(phonexid),
     m_time_transfer(b_engine->timeServer())
{
    if (PendingWidget::counted == UINT_MAX) {
        PendingWidget::counted = 0;
    }
}

void PendingWidget::buildui()
{
    m_layout = new QHBoxLayout(this);
    m_lbl_string = new QLabel(this);
    m_btn_pickup = new QPushButton(tr("Pick up"), this);

    m_layout->addWidget(m_lbl_string);
    m_layout->addWidget(m_btn_pickup);

    connect(m_btn_pickup, SIGNAL(clicked()), this, SLOT(doPickup()));
    connect(this, SIGNAL(remove_me(unsigned int)),
            parent(), SLOT(remove_pending(unsigned int)));
}

void PendingWidget::set_string(const QString & s)
{
    m_lbl_string->setText(s);
}

QString PendingWidget::started_since() const
{
    return b_engine->timeElapsed(m_time_transfer);
}
