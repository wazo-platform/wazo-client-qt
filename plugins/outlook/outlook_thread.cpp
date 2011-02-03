#include "outlook_thread.h"
#include "outlook_com.h"
#include "outlook_engine.h"
#include <windows.h>

static COLThread * gpThread=NULL;

COLThread::COLThread() {
    m_bStop = FALSE;
    gpThread = this;
}

COLThread::~COLThread() {
}

void COLThread::run()
{
    while ( !m_bStop ) {
        DWORD nStart = GetTickCount();

        COLContacts contacts;
        if ( load_contacts_from_outlook(contacts) ) {
            if ( m_bStop )
                return ;

            OLEngine()->update_contacts(contacts);
            if ( m_bStop )
                return ;

            emit contactsLoaded();
        }

        while ( ( GetTickCount()-nStart < OL_REFRESH_INTERVAL ) && !m_bStop )
            msleep(250);
    }
}

bool COLThread::load_contacts_from_outlook(COLContacts & contacts)
{
    int nFolder = OL_FOLDER_CONTACTS;
    COLApp pApp;

    if ( ! pApp.init() ) {
        QString msgtoemit = QString("OutlookErr:%1:%2")
            .arg(pApp.m_init_failure).arg(pApp.m_init_hresult, 0, 16);
        qDebug() << Q_FUNC_INFO << "init error" << msgtoemit;
        emit logClientWarning("COLThread::load_contacts_from_outlook", msgtoemit);
        sleep(1);
        emit errorMessage(msgtoemit);
        return false;
    }

    emit logClientWarning("COLThread::load_contacts_from_outlook", pApp.m_clsid_string);

    COLNameSpace pNS = pApp.GetNamespace("MAPI");
    if ( ! pNS ) {
        QString msgtoemit = "OutlookErr:MAPI";
        qDebug() << Q_FUNC_INFO << "could not get MAPI namespace";
        emit logClientWarning("COLThread::load_contacts_from_outlook", msgtoemit);
        sleep(1);
        emit errorMessage(msgtoemit);
        return false;
    }

    if ( m_bStop ) return false;

    COLFolder pFolder = pNS.GetDefaultFolder(nFolder);
    //	COLFolder pFolder = pNS.PickFolder();

    /*	QString entryID("000000000A8BA4607D4D6040AC4AB3AD73DD80390100BAC11955A683784591695EF73AAA1CFD0000365C00080000");
      QString storeID("0000000038A1BB1005E5101AA1BB08002B2A56C20000454D534D44422E444C4C00000000000000001B55FA20AA6611CD9BC800AA002FC45A0C000000524F4343414D41494C002F6F3D526F636361205472616E73706F7274732F6F753D5072656D6965722067726F7570652061646D696E69737472617469662F636E3D526563697069656E74732F636E3D626F7200");

      COLFolder pFolder=pNS.GetFolderFromID(entryID, storeID);*/

    if ( ! pFolder ) {
        emit errorMessage("OutlookErr:NoFolder");
        return false;
    }

    if ( m_bStop ) return false;
    OutputDebugStringA(pFolder.Name().toAscii());
    OutputDebugStringA("\r\n");

    OutputDebugStringA(pFolder.EntryID().toAscii());
    OutputDebugStringA("\r\n");

    OutputDebugStringA(pFolder.StoreID().toAscii());
    OutputDebugStringA("\r\n");

    COLComContactItems pItems = pFolder.GetItems();

    if ( ! pItems ) {
        emit errorMessage("OutlookErr:NoItems");
        return false;
    }

    COLComContact pContact = pItems.GetFirst();

    while(pContact && !m_bStop)
	{
            COLContact contact;
            if ( !pContact.Load(&contact) )
                return false;
            contacts.append(contact);
            pContact = pItems.GetNext();
	}

    return true;
}
