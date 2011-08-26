#ifndef __AASTRA_SIPNOTIFY_H__
#define __AASTRA_SIPNOTIFY_H__

#include <QVariantMap>

#define SPECIAL_ME "user:special:me"

namespace aastra_notify {

enum AastraKey {
    LINE, KEYPAD, KEYPAD_STAR, KEYPAD_POUND, SOFT_KEY, PRG_KEY,
    TOP_SOFT_KEY, EXP_MOD_SOFT_KEY, VOL_UP, VOL_DOWN, HEADSET, SPEAKER, MUTE,
    XFER, CONF, HOLD, REDIAL, CALLERS, SERVICES, INTERCOM, DIRECTORY, OPTIONS,
    SAVE, DELETE, GOODBYE, VOICEMAIL, NAV_UP, NAV_DOWN, NAV_LEFT, NAV_RIGHT
};

/*! \brief Creates a QVariantMap to simulate a click on an aastra phone
    
    \param type of key that is going to be pressed
    \param channel identifying the phone -- SIP/ewn1j9
    \param num1 used to identify a key when using a type with multiple possibilities
    \param num2 used when multiple numbers are needed
    \return the QVariantMap to emit to send the SIPnotify for that key press
 */
QVariantMap GetKey(AastraKey type, const QString & channel, int num1=0, int num2=0);
/*! \brief Create a QVariantMap to call from an aastra phone

    \param number is the number to call
    \param channel used to dial -- SIP/ewn1j9
    \param interrupt will interrupt ongoing call if true
    \return the QVariantMap to emit the dial SIPnotify
*/
QVariantMap GetDial(const QString & number, const QString & channel, bool interrupt=false);
QVariantMap Build(const QList<QString> &, const QString &);
QString GetKeyUri(AastraKey key, int num1=0, int num2=0);
/*! \brief Add the uris for a number to a given command */
void AppendNumber(const QString & number, QStringList & commands);

} // namespace aastra_notify

#endif // __AASTRA_SIPNOTIFY__

