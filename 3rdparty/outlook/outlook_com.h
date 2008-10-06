#ifndef OUTLOOK_H_INCLUDED
#define OUTLOOK_H_INCLUDED

#ifdef USE_OUTLOOK
#include <windows.h>
#include <QString>
#include <QList>
#include <QHash>

class CIDispatch
{
public:
    CIDispatch(IDispatch * pDisp=NULL, BOOL bAddRef=FALSE){if ( (m_pIDisp=pDisp) && bAddRef ) m_pIDisp->AddRef();}
    CIDispatch(const CIDispatch & disp){if ( m_pIDisp=disp.m_pIDisp ) m_pIDisp->AddRef();}
    virtual ~CIDispatch(){if ( m_pIDisp ) m_pIDisp->Release();}

    virtual void operator=(const CIDispatch & disp){if ( m_pIDisp ) m_pIDisp->Release();if ( m_pIDisp=disp.m_pIDisp ) m_pIDisp->AddRef();}
    virtual void operator=(IDispatch * pDisp){if ( m_pIDisp ) m_pIDisp->Release();if ( m_pIDisp=pDisp ) m_pIDisp->AddRef();}
    operator IDispatch *()const        {return m_pIDisp;}

	BOOL	IsValid()const{return m_pIDisp!=NULL;}
public:
    IDispatch *m_pIDisp;
};

class COLContact : public CIDispatch {
public:
    COLContact(IDispatch * pDisp=NULL, BOOL bAddRef=FALSE):CIDispatch(pDisp, bAddRef){}
	COLContact(const COLContact & disp):CIDispatch(disp){m_properties=disp.m_properties;}
    virtual ~COLContact(){}

	WCHAR* GetPropertyVal(const char * szName);

	bool	operator <(const COLContact & c) const {
		QString str1=m_properties.value("FullName");
		QString str2=c.m_properties.value("FullName");
		return str1<str2;
	}

	void Load();

public:
	QHash<QString, QString> m_properties;
};


class COLContactItems : public CIDispatch {
public:
    COLContactItems(IDispatch * pDisp=NULL, BOOL bAddRef=FALSE):CIDispatch(pDisp, bAddRef){}
    COLContactItems(const COLContactItems & disp):CIDispatch(disp){}
    virtual ~COLContactItems(){}

	COLContact GetFirst();
	COLContact GetNext();
};


class COLFolder : public CIDispatch {
public:
    COLFolder(IDispatch * pDisp=NULL, BOOL bAddRef=FALSE):CIDispatch(pDisp, bAddRef){}
    COLFolder(const COLFolder & disp):CIDispatch(disp){}
    virtual ~COLFolder(){}

	COLContactItems GetItems();
};

class COLNameSpace : public CIDispatch
{
public:
    COLNameSpace(IDispatch * pDisp=NULL, BOOL bAddRef=FALSE):CIDispatch(pDisp, bAddRef){}
    COLNameSpace(const COLNameSpace & disp):CIDispatch(disp){}
    virtual ~COLNameSpace(){}

	COLFolder GetDefaultFolder(UINT nFolder);
};

class COLApp {
public:
    COLApp();
    virtual ~COLApp();


    // properties	
	void          SetVisible(BOOL bVisible);

	COLNameSpace	GetNamespace(const char * szName);
public:
	IDispatch *m_pOutlookApp;
	CLSID clsid;
};

class COLProperty {
public:
	COLProperty(const char * name, const char * display, bool displayable=false) :
		m_strName(name),
		m_strDisplayName(display),
		m_bDisplayable(displayable)
		{}
	virtual ~COLProperty(void) {}

public:
	QString m_strName;
	QString m_strDisplayName;
	bool	m_bDisplayable;
};


/*
    _bstr_t AssistantName;
    IDispatchPtr Parent;
    _bstr_t InternetFreeBusyAddress;
    _bstr_t TelexNumber;
    enum OlObjectClass Class;
    _NameSpacePtr Session;
    VARIANT_BOOL NoAging;
    DATE Anniversary;
    DATE Birthday;
    _bstr_t Hobby;
    _bstr_t MiddleName;
    _bstr_t Title;
    VARIANT_BOOL UnRead;
    enum OlImportance Importance;
    _bstr_t EntryID;
    _bstr_t ComputerNetworkName;
    _bstr_t MessageClass;
    _bstr_t CustomerID;
    _bstr_t FTPSite;
    enum OlGender Gender;
    _bstr_t ManagerName;
    _bstr_t NickName;
    _bstr_t PersonalHomePage;
    _bstr_t BusinessHomePage;
    _bstr_t Profession;
    _bstr_t ReferredBy;
    _bstr_t Spouse;
    _bstr_t TTYTDDTelephoneNumber;
    _bstr_t CompanyMainTelephoneNumber;
    _bstr_t HomeAddressCity;
    _bstr_t HomeAddressCountry;
    _bstr_t HomeAddressPostalCode;
    _bstr_t HomeAddressState;
    _bstr_t HomeAddressStreet;
    _bstr_t HomeAddressPostOfficeBox;
    _bstr_t OtherAddressCity;
    _bstr_t OtherAddressCountry;
    _bstr_t OtherAddressPostalCode;
    _bstr_t OtherAddressState;
    _bstr_t OtherAddressStreet;
    _bstr_t Mileage;
    _bstr_t FullName;
    _bstr_t BillingInformation;
    enum OlSensitivity Sensitivity;
    _bstr_t Subject;
    _bstr_t OtherAddressPostOfficeBox;
    _bstr_t FileAs;
    DATE CreationTime;
    DATE LastModificationTime;
    _bstr_t Companies;
    _bstr_t Children;
    _InspectorPtr GetInspector;
    VARIANT_BOOL IsConflict;
    _bstr_t ConversationIndex;
    _bstr_t UserCertificate;
    _bstr_t LastNameAndFirstName;
    _bstr_t CompanyAndFullName;
    _bstr_t FullNameAndCompany;
    _bstr_t HomeAddress;
    _bstr_t BusinessAddress;
    _bstr_t OtherAddress;
    long OutlookInternalVersion;
    _bstr_t OutlookVersion;
    enum OlMailingAddress SelectedMailingAddress;
    VARIANT_BOOL Journal;
    _bstr_t WebPage;
    _bstr_t YomiFirstName;
    _bstr_t YomiLastName;
    _bstr_t YomiCompanyName;
    _bstr_t LastFirstNoSpace;
    _bstr_t LastFirstSpaceOnly;
    _bstr_t CompanyLastFirstNoSpace;
    _bstr_t CompanyLastFirstSpaceOnly;
    _bstr_t LastFirstNoSpaceCompany;
    _bstr_t LastFirstSpaceOnlyCompany;
    _bstr_t LastFirstAndSuffix;
    _bstr_t LastFirstNoSpaceAndSuffix;
    _bstr_t ConversationTopic;
    enum OlRemoteStatus MarkForDownload;
    _bstr_t BusinessAddressStreet;
    _bstr_t BusinessAddressCity;
    _bstr_t BusinessAddressState;
    _bstr_t BusinessAddressPostalCode;
    _bstr_t BusinessAddressCountry;
    _bstr_t BusinessAddressPostOfficeBox;
    _bstr_t User1;
    _bstr_t User2;
    _bstr_t User3;
    _bstr_t User4;
    long Size;
    FormDescriptionPtr FormDescription;
    _bstr_t NetMeetingAlias;
    _bstr_t Body;
    _bstr_t NetMeetingServer;
    _bstr_t IMAddress;
    VARIANT_BOOL Saved;
    _bstr_t Categories;
    ItemPropertiesPtr ItemProperties;
    _bstr_t Email1DisplayName;
    _bstr_t Email1AddressType;
    _bstr_t Email1Address;
    _bstr_t Email1EntryID;
    _bstr_t Email2DisplayName;
    _bstr_t Email2AddressType;
    _bstr_t Email2Address;
    _bstr_t Email2EntryID;
    AttachmentsPtr Attachments;
    UserPropertiesPtr UserProperties;
    ActionsPtr Actions;
    _bstr_t Email3DisplayName;
    _bstr_t Email3AddressType;
    _bstr_t Email3Address;
    _bstr_t Email3EntryID;
    _bstr_t Account;
    _bstr_t CallbackTelephoneNumber;
    _bstr_t Suffix;
    _bstr_t FirstName;
    _bstr_t GovernmentIDNumber;
    _bstr_t BusinessTelephoneNumber;
    _bstr_t HomeTelephoneNumber;
    _bstr_t Initials;
    LinksPtr Links;
    _bstr_t Language;
    _bstr_t OrganizationalIDNumber;
    _bstr_t LastName;
    enum OlDownloadState DownloadState;
    _bstr_t MailingAddress;
    _bstr_t CompanyName;
    _bstr_t JobTitle;
    _bstr_t Department;
    _bstr_t OfficeLocation;
    _bstr_t PrimaryTelephoneNumber;
    _bstr_t Business2TelephoneNumber;
    _bstr_t MobileTelephoneNumber;
    _bstr_t RadioTelephoneNumber;
    _bstr_t CarTelephoneNumber;
    _bstr_t OtherTelephoneNumber;
    IUnknownPtr MAPIOBJECT;
    _bstr_t PagerNumber;
    _bstr_t OtherFaxNumber;
    _bstr_t BusinessFaxNumber;
    _bstr_t HomeFaxNumber;
    _bstr_t MailingAddressCountry;
    _bstr_t MailingAddressCity;
    _bstr_t MailingAddressState;
    _bstr_t MailingAddressStreet;
    _bstr_t MailingAddressPostalCode;
    _bstr_t MailingAddressPostOfficeBox;
    _ApplicationPtr Application;
    _bstr_t ISDNNumber;
    _bstr_t AssistantTelephoneNumber;
    _bstr_t Home2TelephoneNumber;

*/
class COLProperties : public QList<COLProperty*> {
public:
	COLProperties() {
		append(new COLProperty("FirstName", "First Name"));
		append(new COLProperty("LastName", "Last Name"));
		append(new COLProperty("FullName", "Full Name", true));
		append(new COLProperty("BusinessTelephoneNumber", "Business Phone", true));
		append(new COLProperty("MobileTelephoneNumber", "Mobile Phone", true));
		append(new COLProperty("HomeTelephoneNumber", "Home Phone", true));
		append(new COLProperty("Email1Address", "Email Address", true));
		
/*		append(new COLProperty("CompanyMainTelephoneNumber", "CompanyMainTelephoneNumber"));
		append(new COLProperty("CallbackTelephoneNumber", "CallbackTelephoneNumber"));
		append(new COLProperty("PrimaryTelephoneNumber", "PrimaryTelephoneNumber"));
		append(new COLProperty("Business2TelephoneNumber", "Business2TelephoneNumber"));
		append(new COLProperty("CarTelephoneNumber", "CarTelephoneNumber"));
		append(new COLProperty("RadioTelephoneNumber", "RadioTelephoneNumber"));
		append(new COLProperty("OtherTelephoneNumber", "OtherTelephoneNumber"));
		append(new COLProperty("AssistantTelephoneNumber", "AssistantTelephoneNumber"));
		append(new COLProperty("Home2TelephoneNumber", "Home2TelephoneNumber")); */
	}

	virtual ~COLProperties() {
		while ( !isEmpty() ) delete takeFirst();
	}
};

COLProperties * OLProperties();


class COLContacts : public QList<COLContact*> {
public:
	COLContacts() {
	}

	virtual ~COLContacts() {
		while ( !isEmpty() ) delete takeFirst();
	}
};

COLContacts * OLContacts();

COLContact * OLFindContact(const QString & num);
QString cleanup_num(QString str, bool clean_prefix=false);

#endif // USE_OUTLOOK

#endif // def(OUTLOOK_H_INCLUDED)

