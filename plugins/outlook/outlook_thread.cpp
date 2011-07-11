#include "outlook_thread.h"
#include "outlook_com.h"
#include "outlook_engine.h"
#include <windows.h>

static COLThread * gpThread=NULL;

COLThread::COLThread() {
    m_bStop = FALSE;
    gpThread = this;
}

COLThread::~COLThread()
{
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
        sleep(1);
        emit logClientWarning("COLThread::lcfo", msgtoemit);
        emit errorMessage(msgtoemit);
        return false;
    }

    emit logClientWarning("clsid value", pApp.m_clsid_string);

    COLNameSpace pNS = pApp.GetNamespace("MAPI");
    if ( ! pNS ) {
        QString msgtoemit = "OutlookErr:MAPI";
        qDebug() << Q_FUNC_INFO << "could not get MAPI namespace";
        sleep(1);
        emit logClientWarning("COLThread::lcfo", msgtoemit);
        emit errorMessage(msgtoemit);
        return false;
    }

    if ( m_bStop ) return false;

    COLFolder pFolder = pNS.GetDefaultFolder(nFolder);
    //	COLFolder pFolder = pNS.PickFolder();
    //  COLFolder pFolder=pNS.GetFolderFromID(entryID, storeID);*/

    if ( ! pFolder ) {
        QString msgtoemit = "OutlookErr:NoFolder";
        qDebug() << Q_FUNC_INFO << "no folder";
        sleep(1);
        emit logClientWarning("COLThread::lcfo", msgtoemit);
        emit errorMessage(msgtoemit);
        return false;
    }

    if ( m_bStop ) return false;

    COLComContactItems pItems = pFolder.GetItems();

    if ( ! pItems ) {
        QString msgtoemit = "OutlookErr:NoItems";
        qDebug() << Q_FUNC_INFO << "no item";
        sleep(1);
        emit logClientWarning("COLThread::lcfo", msgtoemit);
        emit errorMessage(msgtoemit);
        return false;
    }

    COLComContact pContact = pItems.GetFirst();

    while(pContact && ! m_bStop) {
        COLContact contact;
        if ( ! pContact.Load(& contact) ) {
            QString msgtoemit = "OutlookErr:LoadContact";
            qDebug() << Q_FUNC_INFO << "unable to load contact";
            sleep(1);
            emit logClientWarning("COLThread::lcfo", msgtoemit);
            emit errorMessage(msgtoemit);
            return false;
        }
        contacts.append(contact);
        pContact = pItems.GetNext();
    }

    QString folderinfo = QString("Outlook Folder : %1").arg(pFolder.Name());
    emit logClientWarning("COLThread::lcfo", folderinfo);

    return true;
}
