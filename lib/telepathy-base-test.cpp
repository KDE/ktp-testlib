/*
 * This file is part of telepathy-integration-daemon
 *
 * Copyright (C) 2009-2010 Collabora Ltd. <info@collabora.co.uk>
 *   @author Dario Freddi <dario.freddi@collabora.co.uk>
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

#include "telepathy-base-test.h"

#include <Soprano/Backend>
#include <Soprano/StorageModel>
#include <Soprano/PluginManager>

#include <Nepomuk/ResourceManager>
#include <Nepomuk/Thing>

#include <nco.h>

#include <TelepathyQt4/PendingAccount>
#include <TelepathyQt4/PendingReady>

#include <KTempDir>

class TelepathyBaseTest::Private
{
    Q_DECLARE_PUBLIC(TelepathyBaseTest)
    TelepathyBaseTest * const q_ptr;
    public:
        Private(TelepathyBaseTest *parent) : q_ptr(parent) {}
        virtual ~Private() {}

        Soprano::Model *model;
        KTempDir *tmpDir;
        Nepomuk::Resource mePersonContact;
};

TelepathyBaseTest::TelepathyBaseTest(QObject* parent)
    : GlibTest(parent)
    , d(new Private(this))
{
}

TelepathyBaseTest::~TelepathyBaseTest()
{
    delete d;
}

void TelepathyBaseTest::initTestCaseImpl()
{
    Tp::Test::initTestCaseImpl();

    // Replace the resource manager to sandbox Nepomuk for this test case
    const Soprano::Backend* backend = Soprano::PluginManager::instance()->discoverBackendByName(QLatin1String("virtuoso"));
    QVERIFY(backend);

    if (backend->supportsFeatures(Soprano::BackendFeatureStorageMemory)) {
        qDebug() << "Using memory storage";
        d->model = backend->createModel(Soprano::BackendSettings() <<
                                        Soprano::BackendSetting(Soprano::BackendOptionStorageMemory));
    } else {
        qDebug() << "Using disk storage";
        d->tmpDir = new KTempDir();
        d->model = backend->createModel(Soprano::BackendSettings() <<
                                                    Soprano::BackendSetting(Soprano::BackendOptionStorageDir,
                                                                            d->tmpDir->name()));
    }

    Nepomuk::ResourceManager::instance()->setOverrideMainModel(d->model);
}

void TelepathyBaseTest::initTestCaseConnectionImpl()
{
    initTestCaseImpl();

    //setupExampleConnection(QLatin1String("account"), QLatin1String(QLatin1String("foobar"), QLatin1String("protocol"), QLatin1String("contactlist"));
//
/*    d->connection = Tp::Connection::create(exampleConnectionData().first, exampleConnectionData().second);

    QVERIFY(connect(d->connection->requestConnect(),
                    SIGNAL(finished(Tp::PendingOperation*)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);
    QCOMPARE(d->connection->isReady(), true);
    qDebug() << d->connection->status();
    QCOMPARE(d->connection->status(), Tp::Connection::StatusConnected);
    */
}

void TelepathyBaseTest::cleanupTestCaseConnectionImpl()
{
    /*
    if (d->connection) {
        // Disconnect and wait for the readiness change
        QVERIFY(connect(d->connection->requestDisconnect(),
                        SIGNAL(finished(Tp::PendingOperation*)),
                        SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
        QCOMPARE(mLoop->exec(), 0);

        if (d->connection->isValid()) {
            QVERIFY(connect(d->connection.data(),
                            SIGNAL(invalidated(Tp::DBusProxy *,
                                               const QString &, const QString &)),
                            mLoop,
                            SLOT(quit())));
            QCOMPARE(mLoop->exec(), 0);
        }
    }
*/
    cleanupTestCaseImpl();
}

void TelepathyBaseTest::cleanupTestCaseImpl()
{
    Tp::GlibTest::cleanupTestCaseImpl();

    if (d->tmpDir) {
        delete d->tmpDir;
    }
}
void TelepathyBaseTest::setupAccountMonitor()
{
/*    d->monitor = new TelepathyAccountMonitor();

    // At this very point, we should have a "me" PIMO person up and running
    // FIXME: Port to new OSCAF standard for accessing "me" as soon as it
    // becomes available.
    Nepomuk::Thing me(QUrl::fromEncoded("nepomuk:/myself"));

    QVERIFY(me.exists());
    // Loop through all the grounding instances of this person
    Q_FOREACH (Nepomuk::Resource resource, me.groundingOccurrences()) {
        // See if this grounding instance is of type nco:contact.
        if (resource.hasType(Nepomuk::Vocabulary::NCO::PersonContact())) {
            // FIXME: We are going to assume the first NCO::PersonContact is the
            // right one. Can we improve this?
            d->mePersonContact = resource;
            break;
        }
    }
    QVERIFY(d->mePersonContact.isValid());
    QVERIFY(me.groundingOccurrences().contains(d->mePersonContact));
    */
}

/*
TelepathyAccountMonitor* TelepathyBaseTest::accountMonitor()
{
    return d->monitor;
}
*/

Nepomuk::Resource TelepathyBaseTest::mePersonContact()
{
    return d->mePersonContact;
}

Soprano::Model* TelepathyBaseTest::mainModel()
{
    return d->model;
}
/*
void TelepathyBaseTest::createAccount()
{
    QVariantMap parameters;
    parameters[QLatin1String("account")] = QLatin1String("foobar");
    Tp::PendingAccount *pacc = accountMonitor()->accountManager()->createAccount(QLatin1String("foo"),
                                              QLatin1String("bar"), QLatin1String("foobar"), parameters);
    QVERIFY(connect(pacc,
                    SIGNAL(finished(Tp::PendingOperation *)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation *))));
    QCOMPARE(mLoop->exec(), 0);
    d->account = pacc->account();
    QVERIFY(d->account);
}

Tp::AccountPtr TelepathyBaseTest::account()
{
    return d->account;
}

Tp::ConnectionPtr TelepathyBaseTest::connection()
{
    return d->connection;
}
*/

#include "telepathy-base-test.moc"

          