#include "VariantToJson.h"
#include "JsonToVariant.h"

#include <QtTest/QtTest>

class VariantToJson : public QObject
{
	Q_OBJECT
	private slots:
		void testFlat()
		{
			QVariantMap map;
			map["string_foo"] = "foo";
			map["bool_true"] = true;
			map["int_42"] = 42;
			map["double_pi"] = 3.14159;

			QString json(JsonQt::VariantToJson::parse(map));
			QCOMPARE(map, JsonQt::JsonToVariant::parse(json).toMap());
		}
		void testComplex()
		{
			QVariantMap map;
			map["string_foo"] = "foo";
			map["bool_true"] = true;
			map["int_42"] = 42;
			map["double_pi"] = 3.14159;
			map["recurse"] = map;
			QVariantList list;
			list.append("zero");
			list.append("one");
			map["list"] = list;

			QString json(JsonQt::VariantToJson::parse(map));
			QCOMPARE(map, JsonQt::JsonToVariant::parse(json).toMap());
		}
};

QTEST_MAIN(VariantToJson);

#include "moc_VariantToJson.cxx"
