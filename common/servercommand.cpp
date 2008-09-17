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
#include <QString>
#include <QStringList>

#include "servercommand.h"

/*! \brief Constructor
 */
ServerCommand::ServerCommand(const QString & vv)
        : m_jsonroot(NULL)
{
        json_error ret = json_parse_document (&m_jsonroot, (char *) qPrintable(vv));
        qDebug() << "ServerCommand::ServerCommand()" << vv << ret;
        
}

ServerCommand::~ServerCommand()
{
        if(m_jsonroot)
                json_free_value (&m_jsonroot);
}

void ServerCommand::display()
{
        if(m_jsonroot)
                recurse( m_jsonroot, 0 );
}

void ServerCommand::recurse(json_t * node, int depth)
{
        while (node != NULL) {
                if (node->text) {
                        if(node->child)
                                qDebug() << depth << node->type << "/" << node->text << ":" << node->child->text << "/";
                        else
                                qDebug() << depth << node->type << "/" << node->text << "//";
                } else {
                        qDebug() << depth << node->type;
                }
                recurse ( node->child, depth + 1 );
                node = node->next;
        }
}

QString ServerCommand::getString(const QString & tlabel)
{
        QString b;
        if(m_jsonroot) {
                json_t * v = json_find_first_label(m_jsonroot, (char *) qPrintable(tlabel));
                if ((v != NULL) && (v->child != NULL))
                        if(v->child->type == JSON_STRING)
                                b = v->child->text;
        }
        return b;
}

QStringList ServerCommand::getStringList(const QString & tlabel)
{
        QStringList b;
        if(m_jsonroot) {
                json_t * v = json_find_first_label(m_jsonroot, (char *) qPrintable(tlabel));
                if ((v != NULL) && (v->child != NULL))
                        if(v->child->type == JSON_ARRAY) {
                                json_t * node = v->child->child;
                                while(node != NULL) {
                                        if(node->type == JSON_STRING)
                                                b.append(node->text);
                                        node = node->next;
                                }
                        }
        }
        return b;
}

QString ServerCommand::find(const QString & tlabel)
{
        QString r;
        char * text = NULL;
        if(m_jsonroot) {
                json_t * v = json_find_first_label(m_jsonroot, (char *) qPrintable(tlabel));
                if ((v != NULL) && (v->child != NULL)) {
                        json_error ret = json_tree_to_string (v->child, &text);
                        if (ret == JSON_OK)
                                r = text;
                }
        }
        return r;
}
