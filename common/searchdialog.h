#ifndef __SEARCHDIALOG_H__
#define __SEARCHDIALOG_H__

#include <QDialog>
#include <QLineEdit>

class SearchDialog : public QDialog
{
    Q_OBJECT
public:
    SearchDialog(QWidget * parent = 0);
    QString text() const { return m_searchText->text(); };
signals:
    void findNext();
private:
    QLineEdit * m_searchText;
};

#endif

