#include <QtTest/QtTest>

class TestUserInfo: public QObject
{
    Q_OBJECT
  public:
    TestUserInfo();
  private slots:
    void ctxor();
    void xid();
    void updateConfig();
    void hasPhoneId();
  private:
    void init();
    QString m_ipbxid;
    QVariantMap m_prop1;
    QStringList m_linelist;
    QStringList m_phoneidlist;
};

