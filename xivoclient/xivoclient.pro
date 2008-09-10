######################################################################
#
# $Revision$
# $Date$
#

COMMONDIR = ../common

SVNVER_  = $$system(touch $${COMMONDIR}/xivoconsts.h mainwidget.cpp && svn info .. | tail -3 | head -1 | sed "s/.*:.//")
SVNVER   = '\\"$${SVNVER_}\\"'          # place quotes around the version string
XIVOVER_ = $$system(cat ../../VERSION)
XIVOVER  = '\\"$${XIVOVER_}\\"'         # place quotes around the version string

DEFINES += SVNVER=\"$${SVNVER}\"    # create a VER macro containing the version string
DEFINES += XIVOVER=\"$${XIVOVER}\"

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . $${COMMONDIR}
CONFIG -= debug
CONFIG += static
CONFIG += uitools
CONFIG += x86 ppc

# Input
HEADERS += mainwidget.h
HEADERS += $${COMMONDIR}/agentdetailspanel.h
HEADERS += $${COMMONDIR}/agentspanel.h
HEADERS += $${COMMONDIR}/astchannel.h
HEADERS += $${COMMONDIR}/baseengine.h
HEADERS += $${COMMONDIR}/callcampaignpanel.h
HEADERS += $${COMMONDIR}/callstackwidget.h
HEADERS += $${COMMONDIR}/callwidget.h
HEADERS += $${COMMONDIR}/conferencepanel.h
HEADERS += $${COMMONDIR}/configwidget.h
HEADERS += $${COMMONDIR}/customerinfopanel.h
HEADERS += $${COMMONDIR}/datetimepanel.h
HEADERS += $${COMMONDIR}/dialpanel.h
HEADERS += $${COMMONDIR}/dirdialog.h
HEADERS += $${COMMONDIR}/directorypanel.h
HEADERS += $${COMMONDIR}/displaymessages.h
HEADERS += $${COMMONDIR}/extendedlabel.h
HEADERS += $${COMMONDIR}/extendedlineedit.h
HEADERS += $${COMMONDIR}/extendedtablewidget.h
HEADERS += $${COMMONDIR}/faxpanel.h
HEADERS += $${COMMONDIR}/identitydisplay.h
HEADERS += $${COMMONDIR}/logeltwidget.h
HEADERS += $${COMMONDIR}/logwidget.h
HEADERS += $${COMMONDIR}/mylocaldirpanel.h
HEADERS += $${COMMONDIR}/parkingpanel.h
HEADERS += $${COMMONDIR}/peerchannel.h
HEADERS += $${COMMONDIR}/peeritem.h
HEADERS += $${COMMONDIR}/peerslayout.h
HEADERS += $${COMMONDIR}/peerwidget.h
HEADERS += $${COMMONDIR}/popup.h
HEADERS += $${COMMONDIR}/queueentrydetailspanel.h
HEADERS += $${COMMONDIR}/queuedetailspanel.h
HEADERS += $${COMMONDIR}/queuespanel.h
HEADERS += $${COMMONDIR}/remotepicwidget.h
HEADERS += $${COMMONDIR}/searchpanel.h
HEADERS += $${COMMONDIR}/servicepanel.h
HEADERS += $${COMMONDIR}/statuspanel.h
HEADERS += $${COMMONDIR}/switchboardwindow.h
HEADERS += $${COMMONDIR}/urllabel.h
HEADERS += $${COMMONDIR}/userinfo.h
HEADERS += $${COMMONDIR}/videopanel.h
HEADERS += $${COMMONDIR}/xivoconsts.h
HEADERS += $${COMMONDIR}/xletprotopanel.h
HEADERS += $${COMMONDIR}/xmlhandler.h

SOURCES += main.cpp
SOURCES += mainwidget.cpp
SOURCES += $${COMMONDIR}/agentdetailspanel.cpp
SOURCES += $${COMMONDIR}/agentspanel.cpp
SOURCES += $${COMMONDIR}/astchannel.cpp
SOURCES += $${COMMONDIR}/baseengine.cpp
SOURCES += $${COMMONDIR}/callcampaignpanel.cpp
SOURCES += $${COMMONDIR}/callstackwidget.cpp
SOURCES += $${COMMONDIR}/callwidget.cpp
SOURCES += $${COMMONDIR}/conferencepanel.cpp
SOURCES += $${COMMONDIR}/configwidget.cpp
SOURCES += $${COMMONDIR}/customerinfopanel.cpp
SOURCES += $${COMMONDIR}/datetimepanel.cpp
SOURCES += $${COMMONDIR}/dialpanel.cpp
SOURCES += $${COMMONDIR}/dirdialog.cpp
SOURCES += $${COMMONDIR}/directorypanel.cpp
SOURCES += $${COMMONDIR}/displaymessages.cpp
SOURCES += $${COMMONDIR}/extendedlabel.cpp
SOURCES += $${COMMONDIR}/extendedlineedit.cpp
SOURCES += $${COMMONDIR}/extendedtablewidget.cpp
SOURCES += $${COMMONDIR}/faxpanel.cpp
SOURCES += $${COMMONDIR}/identitydisplay.cpp
SOURCES += $${COMMONDIR}/logeltwidget.cpp
SOURCES += $${COMMONDIR}/logwidget.cpp
SOURCES += $${COMMONDIR}/mylocaldirpanel.cpp
SOURCES += $${COMMONDIR}/parkingpanel.cpp
SOURCES += $${COMMONDIR}/peerchannel.cpp
SOURCES += $${COMMONDIR}/peeritem.cpp
SOURCES += $${COMMONDIR}/peerslayout.cpp
SOURCES += $${COMMONDIR}/peerwidget.cpp
SOURCES += $${COMMONDIR}/popup.cpp
SOURCES += $${COMMONDIR}/queueentrydetailspanel.cpp
SOURCES += $${COMMONDIR}/queuedetailspanel.cpp
SOURCES += $${COMMONDIR}/queuespanel.cpp
SOURCES += $${COMMONDIR}/remotepicwidget.cpp
SOURCES += $${COMMONDIR}/searchpanel.cpp
SOURCES += $${COMMONDIR}/servicepanel.cpp
SOURCES += $${COMMONDIR}/statuspanel.cpp
SOURCES += $${COMMONDIR}/switchboardwindow.cpp
SOURCES += $${COMMONDIR}/urllabel.cpp
SOURCES += $${COMMONDIR}/userinfo.cpp
SOURCES += $${COMMONDIR}/videopanel.cpp
SOURCES += $${COMMONDIR}/xletprotopanel.cpp
SOURCES += $${COMMONDIR}/xmlhandler.cpp

QT += network
QT += xml
RESOURCES += appli.qrc
TRANSLATIONS = xivoclient_fr.ts
RC_FILE = appli.rc
