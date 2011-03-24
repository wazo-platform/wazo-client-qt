#include <QString>
#include <QVariantMap>

#include "aastrasipnotify.h"

// XML
#define AASTRA_EXECUTE "Content=\
                        <AastraIPPhoneExecute>\
                            <ExecuteItem URI=%1/>\
                        </AastraIPPhoneExecute>"
// URIs
#define PRG_KEY_URI "\"Key:PrgKey%1\""
#define GOODBYE_URI "\"Key:Goodbye\""
#define DIAL_URI "\"Dial:%1\""

// Options
#define INTERRUPT " interruptCall=\"yes\""

QVariantMap getBaseCommand(QString channel, QString content);

QVariantMap getAastraKeyNotify(AastraKey type, QString channel, int num1, int num2)
{
    QString content;
    switch (type) {
    case CALLERS:
    case CONF:
    case DELETE:
    case DIRECTORY:
    case EXP_MOD_SOFT_KEY:
        break;
    case GOODBYE:
        content = QString(AASTRA_EXECUTE).arg(QString(GOODBYE_URI));
        break;
    case HEADSET:
    case HOLD:
    case INTERCOM:
    case KEYPAD:
    case KEYPAD_STAR:
    case KEYPAD_POUND:
    case LINE:
    case MUTE:
    case NAV_DOWN:
    case NAV_LEFT:
    case NAV_RIGHT:
    case NAV_UP:
    case OPTIONS:
        break;
    case PRG_KEY:
        content = QString(AASTRA_EXECUTE).arg(QString(PRG_KEY_URI).arg(num1));
        break;
    case REDIAL:
    case SAVE:
    case SOFT_KEY:
    case SERVICES:
    case SPEAKER:
    case TOP_SOFT_KEY:
    case VOICEMAIL:
    case VOL_UP:
    case VOL_DOWN:
    case XFER:
        break;
    }
    QVariantMap ipbxcommand = getBaseCommand(channel, content);
    return ipbxcommand;
}

QVariantMap getAastraDial(QString number, QString channel, bool interrupt)
{
    QString uri = QString(DIAL_URI).arg(number);
    if (interrupt) uri += INTERRUPT;
    return getBaseCommand(channel, QString(AASTRA_EXECUTE).arg(uri));
}

QVariantMap getBaseCommand(QString channel, QString content)
{
    QVariantMap variables;
    variables["Event"] = "aastra-xml";
    variables["Content-type"] = "application/xml";
    variables["Content"] = content;
    QVariantMap ipbxcommand;
    ipbxcommand["command"] = "sipnotify";
    ipbxcommand["variables"] = variables;
    ipbxcommand["channel"] = channel;
    return ipbxcommand;
}
