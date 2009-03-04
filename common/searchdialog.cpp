
#include <QFormLayout>
#include <QDialogButtonBox>
#include "searchdialog.h"

SearchDialog::SearchDialog(QWidget * parent)
    : QDialog(parent)
{
    QFormLayout * layout = new QFormLayout( this );
    m_searchText  = new QLineEdit();
    layout->addRow( tr("Search :"), m_searchText );
    QDialogButtonBox * buttonBox
        = new QDialogButtonBox( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );
    //connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(accepted()), this, SIGNAL(findNext()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addRow( buttonBox );
}

