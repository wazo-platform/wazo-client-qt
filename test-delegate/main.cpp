#include <QApplication>
#include <QComboBox>
#include <QAbstractItemView>

#include <QHBoxLayout>
#include <QLabel>

#include "delegate.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QComboBox combo ;
    combo.view()->setItemDelegate(new RemoveDelegate);
    combo.view()->setEditTriggers(QAbstractItemView::CurrentChanged);
    combo.addItems(QStringList() << "aaa" << "bbb" << "ccc");
    combo.show();
    
    
    QWidget *w = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout();
    w->setLayout(layout);
    
    QLabel *m_text = new QLabel("test");
    layout->addWidget(m_text);
    
    QLabel *m_button = new QLabel();
    m_button->setMouseTracking(true);
    m_button->setPixmap(QPixmap("./gtk_close.png"));
    layout->addWidget(m_button);
    
    // w->show();
    
    return app.exec();
}
