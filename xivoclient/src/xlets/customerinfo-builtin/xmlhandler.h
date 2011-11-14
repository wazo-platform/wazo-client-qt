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

/* $Revision$
 * $Date$
 */

#ifndef __XMLHANDLER_H__
#define __XMLHANDLER_H__

#include <QXmlDefaultHandler>

class Popup;

/*! \brief for XML Profile parsing
 *
 * This class implements QXmlContentHandler and QXmlErrorHandler.
 * The Popup related object is filled with corresponding data
 * to display.
 */
class XmlHandler: public QXmlDefaultHandler
{
    public:
        XmlHandler(Popup *popup=0);

    private:
        // The following methods are inherited from QXmlContentHandler
        //! for element declaration and attributes handling
        bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts);

        bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);  //! allways called at the close of an element.
        bool characters(const QString &ch);  //! used to handle characters data within elements.
        bool endDocument();  //! called when XML parsing is finished

        // The following methods are inherited from QXmlErrorHandler
        //! called by the XML parser for warnings
        bool warning(const QXmlParseException &exception);
        bool error(const QXmlParseException &exception);  //! called by the XML parser for errors
        bool fatalError(const QXmlParseException &exception);  //! called by the XML parser for fatal errors

    private:
        Popup* m_popup;                //!< pointer to the Popup object to fill with data
        bool m_isParsingInfo;        //!< parsing state : true if we are currently parsing a \<info\> element
        QString m_infoOrder;                //!< type attribute of the current \<info\> element
        QString m_infoType;                //!< type attribute of the current \<info\> element
        QString m_infoName;                //!< name attribute of the current \<info\> element
        QString m_infoValue;        //!< character value of the current \<info\> element
};

#endif
