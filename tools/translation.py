#!/usr/bin/python
# -*- coding: utf-8 -*-

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Alternatively, XiVO CTI Server is available under other licenses directly
# contracted with Pro-formatique SARL. See the LICENSE file at top of the
# source tree or delivered in the installable package in which XiVO CTI Server
# is distributed for more details.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import cjson
import csv
import os
import sys
import xml.dom.minidom

action = sys.argv[1]
inputfiles = sys.argv[2:]

# usage :
# find . -name "*.ts" | xargs ./tools/translation.py makecsv 

fulldict = {}

def parse(language, r):
    for k in r.childNodes[1].childNodes:
        if k.nodeName == 'context':
            for m in k.childNodes:
                if m.nodeName == 'message':
                    srcval = None
                    dstval = None
                    for n in m.childNodes:
                        if n.nodeName == 'source':
                            for pp in n.childNodes:
                                srcval = pp.nodeValue
                        elif n.nodeName == 'translation':
                            for pp in n.childNodes:
                                dstval = pp.nodeValue
                    if dstval is None:
                        dstval = 'UNDEFINED'
                    if srcval not in fulldict[basename][classname]:
                        fulldict[basename][classname][srcval] = {}
                    fulldict[basename][classname][srcval][language] = dstval
                elif m.nodeName == 'name':
                    for n in m.childNodes:
                        classname = n.nodeValue
                        if classname not in fulldict[basename]:
                            fulldict[basename][classname] = {}
    return

def csvout(fulldict):
    k = open('/tmp/trads.csv', 'w')
    x = csv.writer(k, delimiter = ';', quotechar = '"', quoting = csv.QUOTE_ALL)
    # legend
    x.writerow(['Fichier', 'Classe',
                'Anglais (référence)',
                'Français',
                'Allemand',
                'Néerlandais'])
    x.writerow([])
    for basename, classes in fulldict.iteritems():
        for classname, trads in classes.iteritems():
            envalues = sorted(trads.keys())
            for envalue in envalues:
                otherlanguages = trads.get(envalue)
                try:
                    if otherlanguages.get('fr'):
                        frstr = otherlanguages.get('fr')
                    else:
                        frstr = 'UNDEFINED'
                    if otherlanguages.get('de'):
                        destr = otherlanguages.get('de')
                    else:
                        destr = 'UNDEFINED'
                    if otherlanguages.get('nl'):
                        nlstr = otherlanguages.get('nl')
                    else:
                        nlstr = 'UNDEFINED'
                    x.writerow([basename, classname,
                                envalue.encode('utf8'),
                                frstr.encode('utf8'),
                                destr.encode('utf8'),
                                nlstr.encode('utf8')])
                except Exception, exc:
                    print '---', exc, otherlanguages
    k.close()
    return

if action == 'makecsv':
    for infile in inputfiles:
        filename = infile.split('/')[-1]
        (basename, language) = filename[:-3].split('_')
        if basename not in fulldict:
            fulldict[basename] = {}
        r = xml.dom.minidom.parse(infile)
        parse(language, r)
    # print cjson.encode(fulldict)
    csvout(fulldict)
