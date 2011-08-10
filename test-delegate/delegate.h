#include <QItemDelegate>
#include <QLabel>

class RemoveDelegate : public QItemDelegate
{
    Q_OBJECT
    
    public:
        RemoveDelegate(QObject * = NULL);
        QWidget * createEditor(QWidget *,
            const QStyleOptionViewItem &,
            const QModelIndex &) const;
        void setEditorData(QWidget *,
            const QModelIndex &) const;
        void updateEditorGeometry(QWidget *editor,
            const QStyleOptionViewItem &option, const QModelIndex &index) const;
            
    private slots:
        void closed();
    private:
        QLabel *m_text, *m_button;
};
