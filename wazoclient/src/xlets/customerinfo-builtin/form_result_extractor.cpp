/*
 * Copyright 2007-2017 The Wazo Authors  (see the AUTHORS file)
 *
 *
 * This program is free software: you can redistribute it and/or modify
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
 * This programm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

#include "form_result_extractor.h"

QVariantMap FormResultExtractor::extract_form_result(QWidget *form)
{
    QVariantMap form_result;
    QList<QLineEdit *> lineedits = form->findChildren<QLineEdit *>();
    foreach (QLineEdit *lineedit, lineedits) {
        QString key = lineedit->objectName();
        QString value = lineedit->text();
        form_result[key] = value;
    }
    QList<QComboBox *> comboboxes = form->findChildren<QComboBox *>();
    foreach (QComboBox *combobox, comboboxes) {
        QString key = combobox->objectName();
        QString value = combobox->currentText();
        form_result[key] = value;
    }
    QList<QRadioButton *> radiobuttons = form->findChildren<QRadioButton *>();
    foreach (QRadioButton *radiobutton, radiobuttons) {
        QString key = radiobutton->objectName();
        bool value = radiobutton->isChecked();
        form_result[key] = value;
    }
    QList<QDateEdit *> dateedits = form->findChildren<QDateEdit *>();
    foreach (QDateEdit *dateedit, dateedits) {
        QString key = dateedit->objectName();
        QString value = dateedit->date().toString(Qt::ISODate);
        form_result[key] = value;
    }
    QList<QTimeEdit *> timeedits = form->findChildren<QTimeEdit *>();
    foreach (QTimeEdit *timeedit, timeedits) {
        QString key = timeedit->objectName();
        QString value = timeedit->time().toString(Qt::ISODate);
        form_result[key] = value;
    }
    QList<QDateTimeEdit *> datetimeedits = form->findChildren<QDateTimeEdit *>();
    foreach (QDateTimeEdit *datetimeedit, datetimeedits) {
        QString key = datetimeedit->objectName();
        QString value = datetimeedit->dateTime().toString(Qt::ISODate);
        form_result[key] = value;
    }
    QList<QSpinBox *> spinboxes = form->findChildren<QSpinBox *>();
    foreach (QSpinBox *spinbox, spinboxes) {
        QString key = spinbox->objectName();
        int value = spinbox->value();
        form_result[key] = value;
    }
    QList<QDoubleSpinBox *> doublespinboxes = form->findChildren<QDoubleSpinBox *>();
    foreach (QDoubleSpinBox *doublespinbox, doublespinboxes) {
        QString key = doublespinbox->objectName();
        QString value = doublespinbox->cleanText();
        form_result[key] = value;
    }
    QList<QCheckBox *> checkboxes = form->findChildren<QCheckBox *>();
    foreach (QCheckBox *checkbox, checkboxes) {
        QString key = checkbox->objectName();
        bool value = checkbox->isChecked();
        form_result[key] = value;
    }
    QList<QCalendarWidget *> calendarwidgets = form->findChildren<QCalendarWidget *>();
    foreach (QCalendarWidget *calendarwidget, calendarwidgets) {
        QString key = calendarwidget->objectName();
        QString value = calendarwidget->selectedDate().toString(Qt::ISODate);
        form_result[key] = value;
    }
    QList<QLabel *> labels = form->findChildren<QLabel *>();
    foreach (QLabel *label, labels) {
        QString key = label->objectName();
        QString value = label->text();
        form_result[key] = value;
    }
    QList<QPlainTextEdit *> plaintextedits = form->findChildren<QPlainTextEdit *>();
    foreach (QPlainTextEdit *plaintextedit, plaintextedits) {
        QString key = plaintextedit->objectName();
        QString value = plaintextedit->toPlainText();
        form_result[key] = value;
    }

    return form_result;
}
