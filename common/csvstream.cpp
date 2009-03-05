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


