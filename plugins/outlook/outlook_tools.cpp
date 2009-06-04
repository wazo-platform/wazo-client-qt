#include "outlook_tools.h"

WCHAR* wchar_dup(const char * sz, BOOL bSysAlloc) {
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
void wchar_to_qstr(const WCHAR * sz, QString & str) {
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


QString cleanup_num(QString str, bool clean_prefix) {
	str.replace(" ", "");
	str.replace("(", "");
	str.replace(")", "");
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

QString callnum(QString str) {
	str.replace(" ", "");
	str.replace("(", "");
	str.replace(")", "");
	str.replace("+", "");
	str.replace(".", "");

	if ( str.left(2) == QString("33") && str.length() >= 11 ) {
		str.remove(0, 2);
		if ( str.length() == 9 ) {
			str='0'+str;
		}
	}

	return str;
}

bool OLPathExist(const QString & strPath) {
	DWORD fa;
	fa=GetFileAttributesA(strPath.toAscii());
	if (fa==0xFFFFFFFF) return false;
	if (fa&FILE_ATTRIBUTE_DIRECTORY) return true;
    return false;
}

bool OLCreateDirectory(const QString & strPath) {
	// win32 api shits
	QString strTmp("\\\\?\\");
	strTmp+=strPath;

	WCHAR * pszTmp=wchar_dup(strTmp.toAscii());

	bool res=::CreateDirectoryW(pszTmp, NULL);
    free(pszTmp);

	return res;
}

