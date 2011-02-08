#include "outlook_com.h"
#include "outlook_contact.h"
#include "outlook_tools.h"
#include "outlook_engine.h"

#include <stdio.h>
#include <QDebug>


HRESULT AutoWrap(int autoType, VARIANT *pvResult, IDispatch *pDisp, LPOLESTR ptName, int cArgs...)
{
    // Begin variable-argument list...
    va_list marker;
    va_start(marker, cArgs);

    if(! pDisp) {
        qDebug() << "NULL IDispatch passed to AutoWrap()";
        return -1;
    }

    // Variables used...
    DISPPARAMS dp = { NULL, NULL, 0, 0 };
    DISPID dispidNamed = DISPID_PROPERTYPUT;
    DISPID dispID;
    HRESULT hr;
    char buf[200];
    char szName[200];

    // Convert down to ANSI
    WideCharToMultiByte(CP_ACP, 0, ptName, -1, szName, 256, NULL, NULL);

    // Get DISPID for name passed...
    hr = pDisp->GetIDsOfNames(IID_NULL, &ptName, 1, LOCALE_USER_DEFAULT, &dispID);
    if(FAILED(hr)) {
        sprintf(buf, "IDispatch::GetIDsOfNames(\"%s\") failed w/err 0x%08lx", szName, hr);
        qDebug() << buf;
        return hr;
    }

    // Allocate memory for arguments...
    VARIANT *pArgs = new VARIANT[cArgs+1];
    // Extract arguments...
    for(int i=0; i<cArgs; i++) {
        pArgs[i] = va_arg(marker, VARIANT);
    }

    // Build DISPPARAMS
    dp.cArgs = cArgs;
    dp.rgvarg = pArgs;

    // Handle special-case for property-puts!
    if(autoType & DISPATCH_PROPERTYPUT) {
        dp.cNamedArgs = 1;
        dp.rgdispidNamedArgs = &dispidNamed;
    }

    // Make the call!
    hr = pDisp->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT, autoType, &dp, pvResult, NULL, NULL);
    if(FAILED(hr)) {
        sprintf(buf, "IDispatch::Invoke(\"%s\"=%08lx) failed w/err 0x%08lx", szName, dispID, hr);
        qDebug() << buf;
        return hr;
    }
    // End variable-argument section...
    va_end(marker);

    delete [] pArgs;

    return hr;
}

bool COLComContact::Load(COLContact * contact)
{
    HRESULT res;
    QString strPropVal;
    QByteArray array;
    VARIANT result;
    COLPropsDef props_def;
    OLEngine()->get_props_def(props_def);
    COLPropsDef::iterator i;

    for ( i = props_def.begin() ; i != props_def.end() ; ++i ) {
        const COLPropDef & prop = i.value();
        array = prop.m_strName.toAscii();
        WCHAR * wszProp = wchar_dup(array.constData());
        res = AutoWrap(DISPATCH_PROPERTYGET, &result, m_pIDisp, wszProp, 0);
        free(wszProp);

        wchar_to_qstr(result.bstrVal, strPropVal);
        VariantClear(&result);

        if ( FAILED(res) )
            // strPropVal = "(failed)";
            return false;

        contact->m_properties.insert(prop.m_strName, strPropVal);
    }

    res = AutoWrap(DISPATCH_PROPERTYGET, &result, m_pIDisp, L"CreationTime", 0);
    if ( FAILED(res) )
        return false;
    contact->m_dtCreate = result.dblVal;
    VariantClear(&result);

    res = AutoWrap(DISPATCH_PROPERTYGET, &result, m_pIDisp, L"LastModificationTime", 0);
    if ( FAILED(res) )
        return false;
    contact->m_dtLastModified = result.dblVal;
    VariantClear(&result);

    return true;
}


COLComContact COLComContactItems::GetFirst()
{
    VARIANT result;
    HRESULT res;
    res = AutoWrap(DISPATCH_PROPERTYGET, &result, m_pIDisp, L"GetFirst", 0);
    if ( FAILED(res) )
        return NULL;
    return COLComContact(result.pdispVal, FALSE);
}

COLComContact COLComContactItems::GetNext()
{
    VARIANT result;
    HRESULT res;
    res = AutoWrap(DISPATCH_PROPERTYGET, &result, m_pIDisp, L"GetNext", 0);
    if ( FAILED(res) )
        return NULL;
    return COLComContact(result.pdispVal, FALSE);
}

COLComContactItems	COLFolder::GetItems()
{
    VARIANT result;
    HRESULT res;
    res = AutoWrap(DISPATCH_PROPERTYGET, &result, m_pIDisp, L"Items", 0);
    if ( FAILED(res) )
        return NULL;
    return COLComContactItems(result.pdispVal, FALSE);
}

QString COLFolder::Name()
{
    VARIANT result;
    HRESULT res;
    res = AutoWrap(DISPATCH_PROPERTYGET, &result, m_pIDisp, L"Name", 0);

    QString str;
    wchar_to_qstr(result.bstrVal, str);
    VariantClear(&result);

    if ( FAILED(res) )
        return "";
    return str;
}

QString COLFolder::StoreID()
{
    VARIANT result;
    HRESULT res;
    res = AutoWrap(DISPATCH_PROPERTYGET, &result, m_pIDisp, L"StoreID", 0);

    QString str;
    wchar_to_qstr(result.bstrVal, str);
    VariantClear(&result);

    if ( FAILED(res) )
        return "";
    return str;
}

QString COLFolder::EntryID()
{
    VARIANT result;
    HRESULT res;
    res = AutoWrap(DISPATCH_PROPERTYGET, &result, m_pIDisp, L"EntryID", 0);

    QString str;
    wchar_to_qstr(result.bstrVal, str);
    VariantClear(&result);

    if ( FAILED(res) )
        return "";
    return str;
}



COLFolder COLNameSpace::GetDefaultFolder(UINT nFolder)
{
    HRESULT res;
    VARIANT result;
    VARIANT parm;
    parm.vt = VT_I4;
    parm.lVal = nFolder;

    res = AutoWrap(DISPATCH_PROPERTYGET, &result, m_pIDisp, L"GetDefaultFolder", 1, parm);
    VariantClear(&parm);
    if ( FAILED(res) )
        return NULL;

    return COLFolder(result.pdispVal, FALSE);
}

COLFolder COLNameSpace::PickFolder()
{
    VARIANT result;

    HRESULT res = AutoWrap(DISPATCH_PROPERTYGET, &result, m_pIDisp, L"PickFolder", 0);

    if ( FAILED(res) )
        return NULL;
    return COLFolder(result.pdispVal, FALSE);
}

COLFolder COLNameSpace::GetFolderFromID(const QString & strEntryID, const QString & strStoreID)
{
    VARIANT result;
    VARIANT folderID, storeID;
    HRESULT res;

    folderID.vt = VT_BSTR;
    folderID.bstrVal = wchar_dup(strEntryID.toAscii(), TRUE);

    storeID.vt = VT_BSTR;
    storeID.bstrVal = wchar_dup(strStoreID.toAscii(), TRUE);

    // we have to invert entryID and storeID of prototype for correct working
    // Why do we have to pass the optional parameter first???
    res = AutoWrap(DISPATCH_PROPERTYGET, &result, m_pIDisp, L"GetFolderFromID", 2, storeID, folderID);

    VariantClear(&folderID);
    VariantClear(&storeID);

    if ( FAILED(res) )
        return NULL;
    return COLFolder(result.pdispVal, FALSE);
}

COLApp::COLApp()
{
}

bool COLApp::init()
{
    // Initialize COM for this thread...
    m_init_hresult = CoInitialize(NULL);
    if(FAILED(m_init_hresult)) {
        m_init_failure = "com_init";
        return false;
    }

    // Get CLSID for our server...
    m_init_hresult = CLSIDFromProgID(L"Outlook.Application", & m_clsid);
    if(FAILED(m_init_hresult)) {
        m_init_failure = "outlook_install";
        return false;
    }

    m_clsid_string = QUuid(m_clsid).toString();
    qDebug() << Q_FUNC_INFO << "successful clsid match" << m_clsid_string;

    // Start server and get IDispatch...
    m_init_hresult = CoCreateInstance(m_clsid, NULL,
                                      CLSCTX_LOCAL_SERVER, // vs. CLSCTX_ALL ?
                                      IID_IDispatch,
                                      (void **)&m_pOutlookApp);
    if(FAILED(m_init_hresult)) {
        m_init_failure = QString("outlook_register_%1").arg(QUuid(m_clsid).toString());
        return false;
    }

    return true;
}

void COLApp::term()
{
    if ( m_pOutlookApp )
        m_pOutlookApp->Release();
    m_pOutlookApp = NULL;

    // Uninitialize COM for this thread...
    CoUninitialize();
}

COLApp::~COLApp()
{
}

COLNameSpace COLApp::GetNamespace(const char * szName)
{
    VARIANT result;
    VARIANT parm;
    parm.vt = VT_BSTR;
    parm.bstrVal = wchar_dup(szName, TRUE);

    HRESULT res = AutoWrap(DISPATCH_PROPERTYGET, &result, m_pOutlookApp, L"GetNamespace", 1, parm);
    VariantClear(&parm);

    if ( FAILED(res) )
        return NULL;
    return COLNameSpace(result.pdispVal, FALSE);
}
