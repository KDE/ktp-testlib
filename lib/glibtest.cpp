/*
   This file is part of the Chakra project

   Copyright (C) 2010 Dario Freddi <drf@chakra-project.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include "glibtest.h"

#include <telepathy-glib/debug.h>

#include "glib/contactlist/conn.h"

namespace Tp {

class GlibTest::Private
{
public:
    Private() : exampleConnection(0) {}

    ExampleContactListConnection *exampleConnection;
    QPair< QString, QString > exampleConnectionData;
};

GlibTest::GlibTest(QObject* parent)
    : Test(parent)
    , d(new Private)
{

}

GlibTest::~GlibTest()
{
    delete d;
}

void GlibTest::setupExampleConnection(const QString& account, const QString& accountId,
                                      const QString& protocol, const QString& contactList)
{
/*    g_type_init();
    g_set_prgname("conn-roster");
    tp_debug_set_flags("all");
    dbus_g_bus_get(DBUS_BUS_STARTER, 0);

    gchar *name;
    gchar *connPath;
    GError *error = 0;

    d->exampleConnection = EXAMPLE_CONTACT_LIST_CONNECTION(g_object_new(
            EXAMPLE_TYPE_CONTACT_LIST_CONNECTION,
            account.toUtf8().data(), accountId.toUtf8().data(),
            protocol.toUtf8().data(), contactList.toUtf8().data(),
            0));
    QVERIFY(d->exampleConnection != 0);
    QVERIFY(tp_base_connection_register(TP_BASE_CONNECTION(d->exampleConnection),
                "contacts", &name, &connPath, &error));
    QVERIFY(error == 0);

    QVERIFY(name != 0);
    QVERIFY(connPath != 0);

    d->exampleConnectionData = qMakePair(QString::fromLatin1(name), QString::fromLatin1(connPath));

    g_free(name);
    g_free(connPath);
    */
}

QPair< QString, QString > GlibTest::exampleConnectionData() const
{
    return d->exampleConnectionData;
}

void GlibTest::cleanupTestCaseImpl()
{
    if (d->exampleConnection != 0) {
        g_object_unref(d->exampleConnection);
        d->exampleConnection = 0;
    }

    Tp::Test::cleanupTestCaseImpl();
}

}

#include "glibtest.moc"
