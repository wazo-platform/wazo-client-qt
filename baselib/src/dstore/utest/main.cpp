#include <QApplication>

#include "dstore.h"
#include "main.h"

void WatchChange::allTreeChange(const QString &path, DStoreEvent event)
{
    qDebug() << "this " <<  path << " node changed" << event;
}

void WatchChange::specificTreeChange(const QString &path, DStoreEvent event)
{
    qDebug() << "this " <<  path << " node changed" << event;
}

//   /phone/[0-9]*/(number|context|agent-id|user-id)
//   /group/[0-9]*/(group-name|user-list)
//   /state/[0-9]*/(name|color|possible-state-list)
//   /user/[0-9]*/(name|group|state-id)
//   /agent/[0-9]*/(name|number|user-id)
//   /conference/[0-9]*/(name|paused|canal-list)
//   /canal/[0-9]*/(source-id|destination-id|start)

QStringList stateNameList;

void addUser(DStore *tree, int id)
{
    QVariantMap user;
    user.insert("id", id);
    user.insert("name", QString("someUserName%0").arg(id));
    user.insert("state-id", qrand()%3);
    tree->populate(QString("user/%1").arg(id), user);
}

void addPhone(DStore *tree, int pid, int uid)
{
    QVariantMap phone;
    phone.insert("id", pid);
    phone.insert("connected", ((qrand()&2)?1:0));
    phone.insert("user-id", uid);
    tree->populate(QString("phone/%1").arg(pid), phone);
}

void addState(DStore *tree, int id)
{
    QString name = QString("ee");
    stateNameList << name;
    
    QVariantMap state;
    state.insert("id", id);
    state.insert("name", name);
    QVariantMap canChangeTo;
    int i;
    for(i=0;i<3;i++) {
        canChangeTo.insert(QString("%1").arg(i).toUtf8().constData(), QVariant());
    }
    state.insert("changable-to", canChangeTo);
    tree->populate(QString("state/%1").arg(id), state);
}


int main(int argc, char *argv[])
{
    DStore *tree = new DStore();
    WatchChange *dog = new WatchChange;
    
    int i, i2;
    
    for(i=0;i<3;i++) {
        addState(tree, i);
    }
    for(i=0;i<10;i++) {
        addUser(tree, i);
    }
    for(i=0,i2=0;i<5;i++,i2++) {
        addPhone(tree,i2,i);
    }
    for(i=0;i<5;i++,i2++) {
        addPhone(tree,i2,i);
    }
    for(i=4;i<9;i++,i2++) {
        addPhone(tree,i2,i);
    }


    qDebug() << DStoreNode::pp(*tree->root(), 2, 0);

    DStore *result = tree->extract(argv[1]);

    qDebug() << DStoreNode::pp(*result->root(), 2, 0);
    
    delete result;
    delete tree;
    delete dog;
    return 0;
}
