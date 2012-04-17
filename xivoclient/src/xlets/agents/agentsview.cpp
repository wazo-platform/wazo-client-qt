/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

#include <QHeaderView>
#include <QStyleFactory>

#include <baseengine.h>

#include "agentsview.h"

AgentsView::AgentsView(QWidget *parent)
    : QTableView(parent)
{
  setSortingEnabled(true);

  verticalHeader()->setMovable(true);
  verticalHeader()->setFixedWidth(50);

  horizontalHeader()->setMovable(true);
  horizontalHeader()->setSortIndicatorShown(false);
  horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

  setShowGrid(0);
  setAlternatingRowColors(true);
  setSelectionMode(QAbstractItemView::NoSelection);
  setStyleSheet(

      // No decoration of the view itself
      "AgentsView {"
          "border: none;"
          "background: transparent;"
      "}"

      // Remove corner
      "AgentsView QTableCornerButton::section {"
          "background: transparent;"
          "border: none;"
      "}"
  );

  //! \todo see below
  /* This is terrible, but not all styles support to change headers
   * background, but plastique does (probably others, but we have to
   * choose one.
   * Better solutions are welcome (subclassing QHeaderView ?)
   */
  QStyle *plastique = QStyleFactory::create("plastique");
  horizontalHeader()->setStyle(plastique);
  verticalHeader()->setStyle(plastique);

  // Necessary to make the tooltip visible, we can't see the text without this
  horizontalHeader()->setStyleSheet(
      "QToolTip {"
          "color: #000;"
      "}");

}
