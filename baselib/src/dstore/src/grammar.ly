%extra_argument { ParserRet* arg }
%token_type {int}
//%default_type {Token}
%type expr {DStore *}
%type midexpr {QVariantList *}
%type path {QString *}
%type pathfrag {QString *}
%type value {QVariant *}

%include {
    #include "grammar.h"
    #include <assert.h>
    #include "dstore_private.h"

QVariantList flatResult(const QVariantMap &map, const QString &path)
{
    QStringList traverseList = path.split("/");
    QString traverse;
    QVariantMap current = map;
    QVariantList ret;
    int i, e;

    for (i=0,e=traverseList.size()-1;i<e;i++) {
        traverse = traverseList.takeFirst();
        if (traverse == "*") {
            foreach(QString key, current.keys()) {
                ret += flatResult(current[key].toMap(), traverseList.join("/"));
            }
        } else if (current.contains(traverse)) {
            current = current[traverse].toMap();
        }
    }
    traverse = traverseList.takeFirst();

    if (current.contains(traverse)) {
      ret.append(current[traverse]);
    }

    return ret;
}

} 

%syntax_error {
    QString emsg = "syntax error in: ";
    qDebug((emsg + arg->req).toUtf8().constData());
    qDebug((QString(" ").repeated(emsg.size()) +
            QString("_").repeated(arg->count) + "^ after..")
                        .toUtf8().constData());
    arg->abort = 1;
}  
  
treepart ::= expr(A). {
    arg->ret = A;
}
treepart ::= path(B) . {
    arg->ret = arg->origin->extract(*B);
}
treepart ::= . {
    arg->ret = arg->origin->extract("");
}

error ::= . {
  ;
}

midexpr(A) ::= path(B). {
    DStore *tree = arg->origin->extract(*B);
    A = new QVariantList();
    A->append(tree->extractVariant(""));
    delete tree;
    delete B;
}

midexpr(A) ::= expr(B). {
    A = new QVariantList();
    A->append(B->extractVariant(""));
    delete B;
}
midexpr(A) ::= expr(B) SLASH path(C). {
    A = new QVariantList(flatResult(B->extractVMap(""), "*/" + *C));
    delete B;
    delete C;
    arg->count += 1;
}

expr(A) ::= path(B) LC path(C) TEST(D) value(E) RC. {
    A = arg->origin->extract(*B);
    A->filter(D, *C, *E);
    delete B;
    delete C;
    delete E;
    arg->count += 3;
}
expr(A) ::= path(B) LC path(C) TEST(D) midexpr(E) RC. {
    A = arg->origin->extract(*B);
    A->filter(D, *C, *E);
    delete B;
    delete C;
    delete E;
    arg->count += 3;
}


value(A) ::= AT pathfrag(B). {
    A = new QVariant(*B);
    delete B;
}

path(A) ::= pathfrag(B). {
    A = B;
}
path(A) ::= path(B) SLASH pathfrag(C). {
    A = new QString(*B + "/" + *C);
    delete B;
    delete C;
    arg->count += 1;
}

pathfrag(A) ::= CHARACTER(B). {
    A = new QString((char)B);
}
pathfrag(A) ::= pathfrag(B) CHARACTER(C). {
    A = new QString(*B + C);
    delete B;
    arg->count += 1;
}
