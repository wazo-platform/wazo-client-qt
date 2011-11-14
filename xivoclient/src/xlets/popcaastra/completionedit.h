/* XiVO Client
 * Copyright (C) 2011, Avencall
 *
 * This file is part of XiVO Client.
 *
 * XiVO Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with a Section 7 Additional
 * Permission as follows:
 *   This notice constitutes a grant of such permission as is necessary
 *   to combine or link this software, or a modified version of it, with
 *   the OpenSSL project's "OpenSSL" library, or a derivative work of it,
 *   and to copy, modify, and distribute the resulting work. This is an
 *   extension of the special permission given by Trolltech to link the
 *   Qt code with the OpenSSL library (see
 *   <http://doc.trolltech.com/4.4/gpl.html>). The OpenSSL library is
 *   licensed under a dual license: the OpenSSL License and the original
 *   SSLeay license.
 *
 * XiVO Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    FilteredCompleter(QObject * parent);
    /*! \brief Update the content of the completer
     *  \param query The term we are looking for
     */
    void update(const QString & word);
    /*! \brief Get the typed word
     *  \return the word that was typed by the user
     */
    const QString word() const;
    /*! \brief Add an item to the completion list
     *  \param Item to add to the search field
     */
    void insertItem(const QString &);
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
    void setCompleter(FilteredCompleter * c);
    /*! \brief Retreves the QCompleter */
    const FilteredCompleter * completer() const;
protected:
    void keyPressEvent(QKeyEvent * e);
private slots:
    void insertCompletion(const QString & completion);
private:
    FilteredCompleter * c;    //!< Completer
};

#endif // __COMPLETIONEDIT_H__

