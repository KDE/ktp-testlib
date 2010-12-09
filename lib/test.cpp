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

#include "test.h"

#include <cstdlib>

#include <QtCore/QTimer>

#include <TelepathyQt4/Types>
#include <TelepathyQt4/Debug>
#include <TelepathyQt4/DBus>
#include <TelepathyQt4/PendingVoid>
#include <TelepathyQt4/SharedPtr>
#include <TelepathyQt4/RefCounted>

using Tp::PendingOperation;
using Tp::PendingVoid;
using Tp::Client::DBus::PeerInterface;

Test::Test(QObject *parent)
: QObject(parent), mLoop(new QEventLoop(this))
{
    QTimer::singleShot(10 * 60 * 1000, this, SLOT(onWatchdog()));
}

Test::~Test()
{
    delete mLoop;
}

void Test::initTestCaseImpl()
{
    Tp::registerTypes();
    Tp::enableDebug(true);
    Tp::enableWarnings(true);

    QVERIFY(QDBusConnection::sessionBus().isConnected());
}

void Test::initImpl()
{
}

void Test::cleanupImpl()
{
}

void Test::cleanupTestCaseImpl()
{
    // To allow for cleanup code to run (e.g. PendingOperation cleanup after they finish)
    mLoop->processEvents();
}

void Test::expectSuccessfulCall(PendingOperation *op)
{
    if (op->isError()) {
        qWarning().nospace() << op->errorName()
        << ": " << op->errorMessage();
        mLoop->exit(1);
        return;
    }

    mLoop->exit(0);
}

void Test::expectSuccessfulCall(QDBusPendingCallWatcher *watcher)
{
    if (watcher->isError()) {
        qWarning().nospace() << watcher->error().name()
        << ": " << watcher->error().message();
        mLoop->exit(1);
        return;
    }

    mLoop->exit(0);
}

void Test::expectFailure(PendingOperation *op)
{
    if (!op->isError()) {
        qWarning() << "expectFailure(): should have been an error, but wasn't";
        mLoop->exit(1);
        return;
    }

    mLoop->exit(0);
}

void Test::expectSuccessfulProperty(PendingOperation *op)
{
    if (op->isError()) {
        qWarning().nospace() << op->errorName()
        << ": " << op->errorMessage();
        mPropertyValue = QVariant();
        mLoop->exit(1);
    } else {
        Tp::PendingVariant *pv = qobject_cast<Tp::PendingVariant*>(op);
        mPropertyValue = pv->result();
        mLoop->exit(0);
    }
}

void Test::processDBusQueue(Tp::DBusProxy *proxy)
{
    // Call method Ping on the D-Bus Peer interface
    PeerInterface peer(proxy);
    PendingVoid *call = new PendingVoid(peer.Ping(), Tp::SharedPtr<Tp::RefCounted>());

    // Wait for the reply to the Ping call
    while (!call->isFinished()) {
        mLoop->processEvents();
    }

    QVERIFY(call->isFinished());
    QVERIFY(call->isValid());

    // Do one more processEvents so the PendingVoid is always freed
    mLoop->processEvents();
}

void Test::onWatchdog()
{
    // We can't use QFAIL because the test would then go to cleanup() and/or cleanupTestCase(),
    // which would often hang too - so let's just use abort
    qWarning() << "Test took over 10 minutes to finish, it's probably hung up - aborting";
    std::abort();
}


#include "test.moc"

