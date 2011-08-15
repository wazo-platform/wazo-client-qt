#include <QString>
#include <QVariantMap>
#include <QDomDocument>
#include <QDomText>
#include <QDebug>

#include "aastrasipnotify.h"

// XML
#define AASTRA_IP_PHONE_EXECUTE "AastraIPPhoneExecute"
#define AASTRA_EXECUTE_ITEM "ExecuteItem"

// URIs
#define CALLERS_URI "Key:Callers"
#define CONF_URI "Key:Conf"
#define DELETE_URI "Key:Delete"
#define DIAL_URI "Dial:%1"
#define DIRECTORY_URI "Key:Directory"
#define EXP_MODE_SOFT_KEY "ExpMod%1SoftKey%2"
#define GOODBYE_URI "Key:Goodbye"
#define HEADSET_URI "Key:Headset"
#define HOLD_URI "Key:Hold"
#define INTERCOM_URI "Key:Intercom"
#define KEYPAD_URI "Key:KeyPad%1"
#define KEYPAD_POUND_URI "Key:KeyPadPound"
#define KEYPAD_STAR_URI "Key:KeyPadStar"
#define LINE_URI "Key:Line%1"
#define MUTE_URI "Key:Mute"
#define NAV_DOWN_URI "Key:NavDown"
#define NAV_LEFT_URI "Key:NavLeft"
#define NAV_RIGHT_URI "Key:NavRight"
#define NAV_UP_URI "Key:NavUp"
#define OPTIONS_URI "Key:Options"
#define PRG_KEY_URI "Key:PrgKey%1"
#define REDIAL_URI "Key:Redial"
#define SAVE_URI "Key:Save"
#define SERVICES_URI "Key:Services"
#define SOFTKEY_URI "Key:SoftKey%1"
#define SPEAKER_URI "Key:Speaker"
#define TOP_SOFTKEY_URI "Key:TopSoftKey%1"
#define VOICEMAIL_URI "Key:Voicemail"
#define VOL_DOWN_URI "Key:VolDwn"
#define VOL_UP_URI "Key:VolUp"
#define XFER_URI "Key:Xfer"

// Options
#define INTERRUPT "interruptCall"

QVariantMap getBaseCommand(QString channel, QString content);
void setEmptyExecuteDoc(QDomDocument &doc);
void addExecuteElement(QDomDocument &doc, QString uri);

QVariantMap getAastraSipNotify(const QList<QString> & commands, const QString & channel)
{
    QDomDocument content;
    setEmptyExecuteDoc(content);
    for (int i = 0; i < commands.size(); i++) {
        addExecuteElement(content, commands.at(i));
    }
    QVariantMap ipbxcommand = getBaseCommand(channel, content.toString());
    return ipbxcommand;
}

QVariantMap getAastraKeyNotify(AastraKey type, QString channel, int num1, int num2)
{
    QDomDocument content;
    setEmptyExecuteDoc(content);
    addExecuteElement(content, getKeyUri(type, num1, num2));
    QVariantMap ipbxcommand = getBaseCommand(channel, content.toString());
    return ipbxcommand;
}

QVariantMap getAastraDial(QString number, QString channel, bool interrupt)
{
    QDomDocument content;
    setEmptyExecuteDoc(content);
    QString uri = QString(DIAL_URI).arg(number);
    addExecuteElement(content, uri);
    if (interrupt) {
        QDomNodeList list = content.elementsByTagName(AASTRA_EXECUTE_ITEM);
        if (! list.isEmpty()) {
            QDomElement exe = list.item(0).toElement();
            exe.setAttribute(INTERRUPT, "yes");
        }
    }
    return getBaseCommand(channel, content.toString());
}

QVariantMap getBaseCommand(QString channel, QString content)
{
    QVariantMap variables;
    variables["Event"] = "aastra-xml";
    variables["Content-type"] = "application/xml";
    variables["Content"] = "Content=" + content.replace("\n", "");
    QVariantMap ipbxcommand;
    ipbxcommand["command"] = "sipnotify";
    ipbxcommand["variables"] = variables;
    ipbxcommand["channel"] = channel;
    return ipbxcommand;
}

QString getKeyUri(AastraKey key, int num1, int num2)
{
    switch (key) {
    case CALLERS:
        return CALLERS_URI;
    case CONF:
        return CONF_URI;
    case DELETE:
        return DELETE_URI;
    case DIRECTORY:
    case EXP_MOD_SOFT_KEY:
        return QString(EXP_MOD_SOFT_KEY).arg(num1).arg(num2);
    case GOODBYE:
        return GOODBYE_URI;
    case HEADSET:
        return HEADSET_URI;
    case HOLD:
        return HOLD_URI;
    case INTERCOM:
        return INTERCOM_URI;
    case KEYPAD:
        return QString(KEYPAD_URI).arg(num1);
    case KEYPAD_STAR:
        return KEYPAD_STAR_URI;
    case KEYPAD_POUND:
        return KEYPAD_POUND_URI;
    case LINE:
        return QString(LINE_URI).arg(num1);
    case MUTE:
        return MUTE_URI;
    case NAV_DOWN:
        return NAV_DOWN_URI;
    case NAV_LEFT:
        return NAV_LEFT_URI;
    case NAV_RIGHT:
        return NAV_RIGHT_URI;
    case NAV_UP:
        return NAV_UP_URI;
    case OPTIONS:
        return OPTIONS_URI;
    case PRG_KEY:
        return QString(PRG_KEY_URI).arg(num1);
    case REDIAL:
        return REDIAL_URI;
    case SAVE:
        return SAVE_URI;
    case SOFT_KEY:
        return QString(SOFTKEY_URI).arg(num1);
    case SERVICES:
        return SERVICES_URI;
    case SPEAKER:
        return SPEAKER_URI;
    case TOP_SOFT_KEY:
        return QString(TOP_SOFTKEY_URI).arg(num1);
    case VOICEMAIL:
        return VOICEMAIL_URI;
    case VOL_UP:
        return VOL_UP_URI;
    case VOL_DOWN:
        return VOL_DOWN_URI;
    case XFER:
        return XFER_URI;
    default:
        return QString();
    }
}

void setEmptyExecuteDoc(QDomDocument &doc)
{
    QDomElement elem = doc.createElement(AASTRA_IP_PHONE_EXECUTE);
    doc.appendChild(elem);
}

void addExecuteElement(QDomDocument &doc, QString uri)
{
    QDomElement root = doc.documentElement();
    QDomElement e = doc.createElement(AASTRA_EXECUTE_ITEM);
    e.setAttribute("URI", uri);
    root.appendChild(e);
}

