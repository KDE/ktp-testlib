/* Base class for Telepathy-Qt4 based unit tests
 *
 * Copyright (C) 2009-2010 Collabora Ltd. <info@collabora.co.uk>
 * Copyright (C) 2009-2010 Nokia Corporation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "glibtest.h"

#include <telepathy-glib/debug.h>

#include "glib/contactlist/conn.h"


class GLibTest::Private
{
public:
    Private() : exampleConnection(0) {}

    ExampleContactListConnection *exampleConnection;
    QPair< QString, QString > exampleConnectionData;
};

GLibTest::GLibTest(QObject* parent)
    : Test(parent)
    , d(new Private)
{

}

GLibTest::~GLibTest()
{
    delete d;
}

void GLibTest::setupExampleConnection(const QString& account, const QString& accountId,
                                      const QString& protocol, const QString& contactList)
{
    g_type_init();
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

}

QPair< QString, QString > GLibTest::exampleConnectionData() const
{
    return d->exampleConnectionData;
}

void GLibTest::cleanupTestCaseImpl()
{
    if (d->exampleConnection != 0) {
        g_object_unref(d->exampleConnection);
        d->exampleConnection = 0;
    }

    Test::cleanupTestCaseImpl();
}


#include "glibtest.moc"
