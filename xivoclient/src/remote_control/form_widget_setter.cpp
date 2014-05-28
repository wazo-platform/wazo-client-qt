/* XiVO Client
 * Copyright (C) 2007-2013, Avencall
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

#ifdef FUNCTESTS

#include <QDebug>
#include <QCalendarWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDateTime>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QRadioButton>
#include <QPlainTextEdit>
#include <QSpinBox>

#include "form_widget_setter.h"

FormWidgetSetter::FormWidgetSetter(QWidget * form_widget)
{
    this->form_widget = form_widget;
}

FormWidgetSetter::~FormWidgetSetter()
{

}

void FormWidgetSetter::setWidgetValue(const QString widget_name, const QString widget_value)
{
        QList<QLineEdit *> lineedits = this->form_widget->findChildren<QLineEdit *>(widget_name);
        foreach (QLineEdit *lineedit, lineedits) {
            lineedit->setText(widget_value);
        }
        QList<QComboBox *> comboboxes = this->form_widget->findChildren<QComboBox *>(widget_name);
        foreach (QComboBox *combobox, comboboxes) {
            int index = combobox->findText(widget_value);
            combobox->setCurrentIndex(index);
        }
        QList<QRadioButton *> radiobuttons = this->form_widget->findChildren<QRadioButton *>(widget_name);
        foreach (QRadioButton *radiobutton, radiobuttons) {
            radiobutton->setChecked(this->_to_bool(widget_value));
        }
        QList<QDateEdit *> dateedits = this->form_widget->findChildren<QDateEdit *>(widget_name);
        foreach (QDateEdit *dateedit, dateedits) {
            QDate date = QDate::fromString(widget_value, Qt::ISODate);
            dateedit->setDate(date);
        }
        QList<QTimeEdit *> timeedits = this->form_widget->findChildren<QTimeEdit *>(widget_name);
        foreach (QTimeEdit *timeedit, timeedits) {
            QTime time = QTime::fromString(widget_value, "h:m:s");
            timeedit->setTime(time);
        }
        QList<QDateTimeEdit *> datetimeedits = this->form_widget->findChildren<QDateTimeEdit *>(widget_name);
        foreach (QDateTimeEdit *datetimeedit, datetimeedits) {
            QDateTime datetime = QDateTime::fromString(widget_value, Qt::ISODate);
            datetimeedit->setDateTime(datetime);
        }
        QList<QSpinBox *> spinboxes = this->form_widget->findChildren<QSpinBox *>(widget_name);
        foreach (QSpinBox *spinbox, spinboxes) {
            spinbox->setValue(widget_value.toInt());
        }
        QList<QDoubleSpinBox *> doublespinboxes = this->form_widget->findChildren<QDoubleSpinBox *>(widget_name);
        foreach (QDoubleSpinBox *doublespinbox, doublespinboxes) {
            doublespinbox->setValue(widget_value.toDouble());
        }
        QList<QCheckBox *> checkboxes = this->form_widget->findChildren<QCheckBox *>(widget_name);
        foreach (QCheckBox *checkbox, checkboxes) {
            checkbox->setChecked(this->_to_bool(widget_value));
        }
        QList<QCalendarWidget *> calendarwidgets = this->form_widget->findChildren<QCalendarWidget *>(widget_name);
        foreach (QCalendarWidget *calendarwidget, calendarwidgets) {
            QDate date = QDate::fromString(widget_value, Qt::ISODate);
            calendarwidget->setSelectedDate(date);
        }
        QList<QLabel *> labels = this->form_widget->findChildren<QLabel *>(widget_name);
        foreach (QLabel *label, labels) {
            label->setText(widget_value);
        }
        QList<QPlainTextEdit *> plaintextedits = this->form_widget->findChildren<QPlainTextEdit *>(widget_name);
        foreach (QPlainTextEdit *plaintextedit, plaintextedits) {
            plaintextedit->setPlainText(widget_value);
        }
}

bool FormWidgetSetter::_to_bool(const QString & s) {
    return s != "false";
}

#endif
