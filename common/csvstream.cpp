/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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

/* $Revision$
 * $Date$
 */

#include "csvstream.h"

/*! \brief Constructor
 */
CsvStream::CsvStream(QIODevice * device)
    : QTextStream(device),
      fieldSeparator(','), textDelimiter('"')
{
}

/*! \brief read a line
 */
QStringList CsvStream::readRecords()
{
    QStringList records;
    QString record;
    QString line = readLine();
    if(line.size() > 0)
    {
        // autodetect field separator
        if(fieldSeparator == QChar(',')
           && line.count(fieldSeparator)==0
           && line.count(QChar(';')) > 0)
            fieldSeparator = QChar(';');
        // autodetect text delimiter
        if(textDelimiter == QChar('"')
           && line.at(0) == QChar('\''))
            textDelimiter = QChar('\'');
    }
    QString::ConstIterator i = line.begin();
    while(i != line.end())
    {
        if(*i == fieldSeparator)
        {
            records.append(record);
            record.clear();
            i++;
        }
        else if(*i == textDelimiter)
        {
            i++;
            while(i != line.end())
            {
                if(*i == textDelimiter)
                {
                    i++;
                    if((i != line.end()) && (*i == textDelimiter))
                        record.append(*(i++));
                    else
                        break;
                }
                else
                {
                    record.append(*i);
                    i++;
                }
            }
        }
        else do
        {
            record.append(*i);
            i++;
        } while(i != line.end() && *i != fieldSeparator);
    }
    records.append(record);
    return records;
}

/*! \brief write a line in the CSV file
 */
CsvStream & CsvStream::operator<< (const QStringList records)
{
    QString doubleDelimiter = QString(textDelimiter) + textDelimiter;
    for(int i = 0; i < records.size(); i++)
    {
        QString record = records[i];
        if( record.contains(fieldSeparator)
           || record.contains(textDelimiter) )
        {
            record = textDelimiter
                     + record.replace(textDelimiter, doubleDelimiter)
                     + textDelimiter;
        }
        (QTextStream &)*this << record;
        if(i == records.size() - 1)
            (QTextStream &)*this << QChar('\n');
        else
            (QTextStream &)*this << fieldSeparator;
    }
    return *this;
}


