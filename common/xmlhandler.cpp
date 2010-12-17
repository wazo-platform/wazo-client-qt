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

#include <QDebug>

#include "xmlhandler.h"
#include "popup.h"

/*!
 * Basic constructor
 * \param popup Popup widget related to the XML stream
 */
XmlHandler::XmlHandler( Popup *popup )
    : m_popup(popup), m_isParsingInfo(false)
{
    qDebug() << Q_FUNC_INFO;
}

/*!
 * Start of element callback.
 *
 * Detect "info" element and store "type" and "name"
 * attributes.
 */
bool XmlHandler::startElement( const QString & /*namespaceURI*/,
                               const QString & localName,
                               const QString & /*qName*/,
                               const QXmlAttributes & atts )
{
    // qDebug() << Q_FUNC_INFO << localName << namespaceURI << qName;
    if( localName == "sheet_info" ) {
        m_isParsingInfo = true;
        m_infoOrder = atts.value("order");
        m_infoName  = atts.value("name");
        m_infoType  = atts.value("type");
        m_infoValue = "";
    } else if( localName == "systray_info" ) {
        m_isParsingInfo = true;
        m_infoOrder = atts.value("order");
        m_infoName  = "";
        m_infoType  = atts.value("type");
        m_infoValue = "";
    } else if( localName == "action_info" ) {
        m_isParsingInfo = true;
        m_infoOrder = atts.value("order");
        m_infoName  = atts.value("name");
        m_infoType  = atts.value("type");
        m_infoValue = "";
    } else if( localName == "sheet_qtui" ) {
        m_isParsingInfo = true;
        m_infoName  = atts.value("name");
        m_infoType  = atts.value("type");
        m_infoValue = "";
    } else if( localName == "internal" ) {
        m_isParsingInfo = true;
        m_infoOrder = "";
        m_infoName  = atts.value("name");
        m_infoType  = atts.value("type");
        m_infoValue = "";
    } else {
        m_isParsingInfo = false;
    }
    return true;
}

/*!
 * Called when a XML element is closed :
 * use the data stored in attributes to create
 * widgets in the "popup"
 */
bool XmlHandler::endElement( const QString & /*namespaceURI*/,
                             const QString & localName,
                             const QString & /*qName*/)
{
    // qDebug() << Q_FUNC_INFO << localName << m_infoOrder << m_infoType << m_infoName << m_infoValue;
    m_isParsingInfo = false;
    if(m_popup == NULL)
        return true;

    if( (localName == "sheet_info") ||
        (localName == "systray_info") ||
        (localName == "action_info" ) ||
        (localName == "internal") )
        m_popup->addAnyInfo(localName, m_infoOrder, m_infoType, m_infoName, m_infoValue);
    else if( localName == "sheet_qtui" )
        m_popup->addDefForm(m_infoName, m_infoValue);
    else if( localName == "profile" )
        m_popup->finishAndShow();

    return true;
}

bool XmlHandler::characters( const QString & ch )
{
    // qDebug() << Q_FUNC_INFO << "chars=" << ch;
    if(m_isParsingInfo)
        m_infoValue.append(ch);
    return true;
}

bool XmlHandler::endDocument()
{
    qDebug() << Q_FUNC_INFO;
    //if(m_popup)
    //        m_popup->finishAndShow();
    return true;
}

// QXmlErrorHandler
bool XmlHandler::warning( const QXmlParseException & exception )
{
    qDebug() << Q_FUNC_INFO
             << exception.message();
    return true;
}

bool XmlHandler::error( const QXmlParseException & exception )
{
    qDebug() << Q_FUNC_INFO
             << exception.message();
    return true;
}

bool XmlHandler::fatalError( const QXmlParseException & exception )
{
    qDebug() << Q_FUNC_INFO
             << exception.message()
             << exception.columnNumber()
             << exception.lineNumber();
    return true;
}
