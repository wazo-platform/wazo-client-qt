/* XIVO CTI clients
 * Copyright (C) 2007-2009  Proformatique
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
	//! Constructor
	XmlHandler( Popup * popup=0 );
private:
	// The following methods are inherited from QXmlContentHandler
	//! for element declaration and attributes handling
	bool startElement( const QString & namespaceURI,
	                   const QString & localName,
	                   const QString & qName,
	                   const QXmlAttributes & atts );
	//! allways called at the close of an element.
	bool endElement( const QString & namespaceURI,
	                 const QString & localName,
	                 const QString & qName );
	//! used to handle characters data within elements.
	bool characters( const QString & ch );
	//! called when XML parsing is finished
	bool endDocument();

	// The following methods are inherited from QXmlErrorHandler
	//! called by the XML parser for warnings
	bool warning( const QXmlParseException & exception );
	//! called by the XML parser for errors
	bool error( const QXmlParseException & exception );
	//! called by the XML parser for fatal errors
	bool fatalError( const QXmlParseException & exception );
private:
	Popup * m_popup;		//!< pointer to the Popup object to fill with data
	bool m_isParsingInfo;	//!< parsing state : true if we are currently parsing a \<info\> element
	QString m_infoOrder;		//!< type attribute of the current \<info\> element
	QString m_infoType;		//!< type attribute of the current \<info\> element
	QString m_infoName;		//!< name attribute of the current \<info\> element
	QString m_infoValue;	//!< character value of the current \<info\> element
};

#endif
