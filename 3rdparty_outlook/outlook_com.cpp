#ifdef USE_OUTLOOK

#include "outlook_com.h"

#include <stdio.h>

#define OL_FOLDER_CONTACTS 10

static COLProperties * gpProperties=NULL;
COLProperties * OLProperties() {
	if ( !gpProperties ) gpProperties=new COLProperties;
	return gpProperties;
}

void InitOLContacts();

static COLContacts * gpContacts=NULL;
COLContacts * OLContacts() {
	if ( !gpContacts ) InitOLContacts();
	return gpContacts;
}

QString cleanup_num(QString str, bool clean_prefix) {
	str.replace(" ", "");
	str.replace("(", "");
	str.replace("+", "");
	str.replace(".", "");

	if ( clean_prefix ) {
		if ( str.left(2) == QString("33") ) {
			str.remove(0, 2);
			str='0'+str;
		}

		if ( str.length() == 9 ) {
			str='0'+str;
		}
	}
	return str;
}


void InitOLContacts() {

	gpContacts=new COLContacts;
	COLApp pApp;
	COLNameSpace pNS = pApp.GetNamespace("MAPI");

	COLFolder pFolder = pNS.GetDefaultFolder(OL_FOLDER_CONTACTS);

	if ( !pFolder ) {
		// bla bla bla
		return ;
	}

	COLContactItems pItems = pFolder.GetItems();

	if ( !pItems ) {
		// bla bla bla
		return ;
	}

	COLContact pContact=pItems.GetFirst();

	while(pContact)
	{
		pContact.Load();
		COLContact * pNewContact = new COLContact(pContact);
		gpContacts->append(pNewContact);
		pNewContact->m_pIDisp->Release();
		pNewContact->m_pIDisp=NULL;
		pContact=pItems.GetNext();
	}
	// qSort(*gpContacts);
}

HRESULT AutoWrap(int autoType, VARIANT *pvResult, IDispatch *pDisp, LPOLESTR ptName, int cArgs...) {
    // Begin variable-argument list...
    va_list marker;
    va_start(marker, cArgs);

    if(!pDisp) {
        MessageBoxA(NULL, "NULL IDispatch passed to AutoWrap()", "Error", 0x10010);
#ifdef EXIT_ON_ERROR
        _exit(0);
		return -1;
#endif
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
        MessageBoxA(NULL, buf, "AutoWrap()", 0x10010);
        _exit(0);
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
        MessageBoxA(NULL, buf, "AutoWrap()", 0x10010);
#ifdef EXIT_ON_ERROR
        _exit(0);
#endif
        return hr;
    }
    // End variable-argument section...
    va_end(marker);

    delete [] pArgs;
	
    return hr;
}

static 	WCHAR* wchar_dup(const char * sz, BOOL bSysAlloc=FALSE) {
    int len=strlen(sz);
    WCHAR * pTemp=(WCHAR*)malloc(sizeof(WCHAR)*(len+1));
    if ( (len+1) != MultiByteToWideChar(CP_ACP, 0, sz, -1, pTemp, len+1))
    {
       	 free(pTemp);
//		 HLog(HLOG_ERROR, "HStrOleStr: Internal Error");
	   	 return NULL;
	}

	WCHAR* ret;
	if ( bSysAlloc ) {
		ret=::SysAllocString(pTemp);
		free(pTemp);
	}
	else {
		ret=pTemp;
	}
	return ret;
}

// TODO: use QString internal buffer if possible
static void wchar_to_qstr(const WCHAR * sz, QString & str) {
	if ( sz ) {
		
		int len=wcslen(sz);
		CHAR * pTemp=(CHAR*)malloc(sizeof(CHAR)*(len+1));
	    if ( (len+1) == WideCharToMultiByte(CP_ACP, 0, sz, -1, pTemp, len+1, NULL, NULL)) {
			str=pTemp;
		} else {
	   		// bla bla bla error
			str="";
		}
		free(pTemp);
	} else
		str="";
}

WCHAR* COLContact::GetPropertyVal(const char * szName) {
	WCHAR * prop=wchar_dup(szName);
	VARIANT result;
	AutoWrap(DISPATCH_PROPERTYGET, &result, m_pIDisp, prop, 0);
	free(prop);
	return result.bstrVal;
}

void COLContact::Load() {
	QByteArray array;
	COLProperties * props=OLProperties();
	for ( int i = 0, c = props->count() ; i < c ; i++ ) {
		COLProperty * pProp=props->at(i);
		array=pProp->m_strName.toAscii();
		WCHAR * prop=wchar_dup(array.constData());
		VARIANT result;
		AutoWrap(DISPATCH_PROPERTYGET, &result, m_pIDisp, prop, 0);
		free(prop);
		QString strPropVal;
		wchar_to_qstr(result.bstrVal, strPropVal);
		VariantClear(&result);

		m_properties.insert(pProp->m_strName, strPropVal);
	}
}


COLContact	COLContactItems::GetFirst() {
	VARIANT result;
	AutoWrap(DISPATCH_PROPERTYGET, &result, m_pIDisp, L"GetFirst", 0);
	return COLContact(result.pdispVal, FALSE);
}

COLContact	COLContactItems::GetNext() {
	VARIANT result;
	AutoWrap(DISPATCH_PROPERTYGET, &result, m_pIDisp, L"GetNext", 0);
	return COLContact(result.pdispVal, FALSE);
}

COLContactItems	COLFolder::GetItems() {
	VARIANT result;
	AutoWrap(DISPATCH_PROPERTYGET, &result, m_pIDisp, L"Items", 0);
	return COLContactItems(result.pdispVal, FALSE);
}


COLFolder	COLNameSpace::GetDefaultFolder(UINT nFolder) {
	VARIANT result;
    VARIANT parm;
	parm.vt = VT_I4;
	parm.lVal = nFolder;

	AutoWrap(DISPATCH_PROPERTYGET, &result, m_pIDisp, L"GetDefaultFolder", 1, parm);
    VariantClear(&parm);

	return COLFolder(result.pdispVal, FALSE);
}


COLApp::COLApp()
{
   // Initialize COM for this thread...
   CoInitialize(NULL);

   // Get CLSID for our server...
   HRESULT hr = CLSIDFromProgID(L"Outlook.Application", &clsid);

   if(FAILED(hr)) {

      ::MessageBoxA(NULL, "Are you sure outlook is installed on this station?!?", "Error", 0x10010);
      return ;
   }

   // Start server and get IDispatch...
   hr = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void **)&m_pOutlookApp);
   if(FAILED(hr)) {
      ::MessageBoxA(NULL, "Outlook is not registered properly", "Error", 0x10010);
      return ;
   }
}

COLApp::~COLApp()
{
	if ( m_pOutlookApp )
		m_pOutlookApp->Release();

   // Uninitialize COM for this thread...
   CoUninitialize();
   
}

COLNameSpace	COLApp::GetNamespace(const char * szName) {
	VARIANT result;
    VARIANT parm;
    parm.vt = VT_BSTR;
    parm.bstrVal = wchar_dup(szName, TRUE);

	AutoWrap(DISPATCH_PROPERTYGET, &result, m_pOutlookApp, L"GetNamespace", 1, parm);
    VariantClear(&parm);

	return COLNameSpace(result.pdispVal, FALSE);
}

COLContact * OLFindContact(const QString & num) {
	COLProperties * pProperties = OLProperties();
	COLContacts * pContacts=OLContacts();
	QString strNum=cleanup_num(num, true);
	for ( int i = 0, c = pContacts->count() ; i < c ; i++ ) {
		COLContact * pContact=(*pContacts)[i];
		for ( int j = 0, d = pProperties->count() ; j < d ; j++ ) {
			QString val=cleanup_num(pContact->m_properties.value((*pProperties)[j]->m_strName), true);
			if ( val == strNum )
				return pContact;
		}
	}
	return NULL;
}

#endif // USE_OUTLOOK
