/* XIVO CTI clients
 * Copyright (C) 2007, 2008  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
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
	qDebug() << "XmlHandler::XmlHandler()";
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
        // qDebug() << "XmlHandler::startElement()" << localName << namespaceURI << qName;
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
        // qDebug() << "XmlHandler::endElement()" << localName << m_infoOrder << m_infoType << m_infoName << m_infoValue;
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
	//qDebug() << "chars=" << ch;
	if(m_isParsingInfo)
		m_infoValue.append(ch);
	return true;
}

bool XmlHandler::endDocument()
{
	qDebug() << "XmlHandler::endDocument()";
	//if(m_popup)
	//	m_popup->finishAndShow();
	return true;
}

// QXmlErrorHandler
bool XmlHandler::warning( const QXmlParseException & exception )
{
	qDebug() << "XmlHandler::warning() " << exception.message();
	return true;
}

bool XmlHandler::error( const QXmlParseException & exception )
{
	qDebug() << "XmlHandler::error() " << exception.message();
	return true;
}

bool XmlHandler::fatalError( const QXmlParseException & exception )
{
	qDebug() << "XmlHandler::fatalError() " << exception.message() << exception.columnNumber() << exception.lineNumber();
	return true;
}
