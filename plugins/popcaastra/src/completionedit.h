#ifndef __COMPLETIONEDIT_H__
#define __COMPLETIONEDIT_H__

#include <QCompleter>
#include <QLineEdit>
#include <QString>
#include <QStringList>
#include <QStringListModel>

/*! \brief A QCompleter with a filter option
 *
 *  This completer add a filter method to remove entries from the completion
 *  list to match the user input
 */
class FilteredCompleter : public QCompleter
{
    Q_OBJECT

public:
    /*! \brief Construct a FilterCompleter
     *  \param list The list of words available for completion
     *  \param parent The parent widget
     */
    FilteredCompleter(const QStringList & list, QObject * parent);
    /*! \brief Update the content of the completer
     *  \param query The term we are looking for
     */
    void update(const QString & word);
    /*! \brief Get the typed word
     *  \return the word that was typed by the user
     */
    const QString word() const;
private:
    QStringList m_list;         //!< Static content
    QStringListModel m_model;   //!< Completion model
    QString m_word;             //!< Current entry
};

/*! \brief A filtered line edit
 *
 *  A line edit with a content filter based on input
 *  The content is matched at any position
 *  ex: filtering "cal" will match "local"
 */
class FilteredLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    FilteredLineEdit(QWidget *parent = 0);
    /*! \brief Set a completer for the line edit
     *  \param c a QCompleter
     */
    void setCompleter(QCompleter * c);
    /*! \brief Retreves the QCompleter */
    const QCompleter * completer() const;
protected:
    void keyPressEvent(QKeyEvent * e);
private slots:
    void insertCompletion(const QString & completion);
private:
    QCompleter * c;    //!< Completer
};

#endif // __COMPLETIONEDIT_H__

