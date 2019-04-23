/**
 ** This file is part of the WiFi project.
 ** Copyright 2019 张作深 <zhangzuoshen@hangsheng.com.cn>.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include <QtTest/QtTest>

// add necessary includes here
#include <WiFi/wifimacaddress.h>

class WiFiMacAddressUnit : public QObject
{
    Q_OBJECT

public:
    WiFiMacAddressUnit();
    ~WiFiMacAddressUnit();

public slots:
    void initTestCase();
    void init();
    void cleanup();

private slots:
    void test_null();

    void test_create_data();
    void test_create();

    void test_assignment();

    void test_comparison_data();
    void test_comparison();

    void test_lessThan_data();
    void test_lessThan();

    void test_clear_data();
    void test_clear();
};

WiFiMacAddressUnit::WiFiMacAddressUnit()
{

}

WiFiMacAddressUnit::~WiFiMacAddressUnit()
{

}

void WiFiMacAddressUnit::initTestCase()
{

}

void WiFiMacAddressUnit::init()
{

}

void WiFiMacAddressUnit::cleanup()
{

}

void WiFiMacAddressUnit::test_null()
{
    {
        WiFiMacAddress address;
        QVERIFY2(address.isNull(), "WiFiMacAddress() should be null.");
    }
    {
        QString empty;
        WiFiMacAddress address(empty);
        QVERIFY(address.isNull());
    }
    {
        WiFiMacAddress address("00:00:00:00:00:00");
        QVERIFY2(address.isNull(), "WiFiMacAddress() should be null.");
    }
}

void WiFiMacAddressUnit::test_create_data()
{
    QTest::addColumn<QString>("expected");
    QTest::addColumn<quint64>("addressUint64");
    QTest::addColumn<QString>("addressS12");
    QTest::addColumn<QString>("addressS17");

    QTest::newRow("00:00:00:00:00:00") << QString("00:00:00:00:00:00")
                                       << Q_UINT64_C(0x000000000000)
                                       << QString("000000000000")
                                       << QString("00:00:00:00:00:00");
    QTest::newRow("11:22:33:44:55:66") << QString("11:22:33:44:55:66")
                                       << Q_UINT64_C(0x112233445566)
                                       << QString("112233445566")
                                       << QString("11:22:33:44:55:66");
    QTest::newRow("AA:BB:CC:DD:EE:FF") << QString("AA:BB:CC:DD:EE:FF")
                                       << Q_UINT64_C(0xAABBCCDDEEFF)
                                       << QString("AABBCCDDEEFF")
                                       << QString("AA:BB:CC:DD:EE:FF");
    QTest::newRow("aa:bb:cc:dd:ee:ff") << QString("AA:BB:CC:DD:EE:FF")
                                       << Q_UINT64_C(0xaabbccddeeff)
                                       << QString("aabbccddeeff")
                                       << QString("aa:bb:cc:dd:ee:ff");
    QTest::newRow("FF:FF:FF:FF:FF:FF") << QString("FF:FF:FF:FF:FF:FF")
                                       << Q_UINT64_C(0xFFFFFFFFFFFF)
                                       << QString("FFFFFFFFFFFF")
                                       << QString("FF:FF:FF:FF:FF:FF");
}

void WiFiMacAddressUnit::test_create()
{
    QFETCH(QString, expected);
    QFETCH(quint64, addressUint64);
    QFETCH(QString, addressS12);
    QFETCH(QString, addressS17);
    {
        WiFiMacAddress address(addressUint64);
        QCOMPARE(address.toString(), expected);
    }
    {
        WiFiMacAddress address(addressS12);
        QCOMPARE(address.toString(), expected);
    }
    {
        WiFiMacAddress address(addressS17);
        QCOMPARE(address.toString(), expected);
    }
    {
        WiFiMacAddress address(expected);
        WiFiMacAddress copy(address);
        QCOMPARE(address.toString(), copy.toString());
    }
}

void WiFiMacAddressUnit::test_assignment()
{
    WiFiMacAddress address(Q_UINT64_C(0x112233445566));

    {
        WiFiMacAddress copy = address;

        QCOMPARE(address.toUInt64(), copy.toUInt64());
    }

    {
        WiFiMacAddress copy1;
        WiFiMacAddress copy2;

        QVERIFY(copy1.isNull());
        QVERIFY(copy2.isNull());

        copy1 = copy2 = address;

        QVERIFY(!copy1.isNull());
        QVERIFY(!copy2.isNull());

        QVERIFY(address.toUInt64() == copy1.toUInt64());
        QVERIFY(address.toUInt64() == copy2.toUInt64());

        copy1.clear();
        QVERIFY(copy1.isNull());
        QVERIFY2(copy1 != address,
                 "Verify that copy1 is a copy of address, the d_ptr are being copied");
    }
}


void WiFiMacAddressUnit::test_comparison_data()
{
    QTest::addColumn<WiFiMacAddress>("address1");
    QTest::addColumn<WiFiMacAddress>("address2");
    QTest::addColumn<bool>("result");

    QTest::newRow("00:00:00:00:00:00 == 00:00:00:00:00:00 = true")
            << WiFiMacAddress()
            << WiFiMacAddress()
            << true;
    QTest::newRow("11:22:33:44:55:66 == 00:00:00:00:00:00 = false")
            << WiFiMacAddress("11:22:33:44:55:66")
            << WiFiMacAddress()
            << false;
    QTest::newRow("11:22:33:44:55:66 == 11:22:33:44:55:66 = true")
            << WiFiMacAddress("11:22:33:44:55:66")
            << WiFiMacAddress("11:22:33:44:55:66")
            << true;
}

void WiFiMacAddressUnit::test_comparison()
{
    QFETCH(WiFiMacAddress, address1);
    QFETCH(WiFiMacAddress, address2);
    QFETCH(bool, result);

    QCOMPARE(address1 == address2, result);
    QCOMPARE(address2 == address1, result);
    QCOMPARE(address1 != address2, !result);
    QCOMPARE(address2 != address1, !result);
}

void WiFiMacAddressUnit::test_lessThan_data()
{
    QTest::addColumn<WiFiMacAddress>("address1");
    QTest::addColumn<WiFiMacAddress>("address2");
    QTest::addColumn<bool>("result");

    QTest::newRow("00:00:00:00:00:00 < 00:00:00:00:00:00 = false")
            << WiFiMacAddress()
            << WiFiMacAddress()
            << false;
    QTest::newRow("00:00:00:00:00:00 < 11:22:33:44:55:66 = true")
            << WiFiMacAddress()
            << WiFiMacAddress("11:22:33:44:55:66")
            << true;
    QTest::newRow("11:22:33:44:55:66 < 00:00:00:00:00:00 = false")
            << WiFiMacAddress("11:22:33:44:55:66")
            << WiFiMacAddress()
            << false;
    QTest::newRow("11:22:33:44:55:66 < AA:BB:CC:DD:EE:FF = true")
            << WiFiMacAddress("11:22:33:44:55:66")
            << WiFiMacAddress("AA:BB:CC:DD:EE:FF")
            << true;
    QTest::newRow("AA:BB:CC:DD:EE:FF < 11:22:33:44:55:66 = false")
            << WiFiMacAddress("AA:BB:CC:DD:EE:FF")
            << WiFiMacAddress("11:22:33:44:55:66")
            << false;
}

void WiFiMacAddressUnit::test_lessThan()
{
    QFETCH(WiFiMacAddress, address1);
    QFETCH(WiFiMacAddress, address2);
    QFETCH(bool, result);

    QCOMPARE(address1 < address2, result);
}

void WiFiMacAddressUnit::test_clear_data()
{
    QTest::addColumn<QString>("addressS17");

    QTest::newRow("FF:00:F3:25:00:00") << QString("FF:00:F3:25:00:00");
}

void WiFiMacAddressUnit::test_clear()
{
    QFETCH(QString, addressS17);
    WiFiMacAddress address(addressS17);
    address.clear();
    QVERIFY(address.toString() == QString("00:00:00:00:00:00"));
}


QTEST_APPLESS_MAIN(WiFiMacAddressUnit)

#include "tst_wifimacaddressunit.moc"
