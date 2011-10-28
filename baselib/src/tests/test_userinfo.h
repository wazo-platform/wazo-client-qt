#include <QtTest/QtTest>

class TestUserInfo: public QObject
{
    Q_OBJECT
  public:
    TestUserInfo();
  private slots:
    void constructor();
    void xid();
    void updateConfig();
    void updateStatus();
    void setAvailState();
    void hasPhoneId();
    void hasChannelId();
    void findNumberForXChannel();
    void xchannels();
    void isTalkingTo();
    void identitylist();
  private:
    void init();
    QString m_ipbxid;
    QVariantMap m_prop1;
    QStringList m_linelist;
    QStringList m_phoneidlist;
};

