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

#ifndef TELEPATHY_TESTLIB_TEST_H
#define TELEPATHY_TESTLIB_TEST_H

#include "telepathy_testlib_export.h"

#include <QtDBus>

#include <QtTest>

#include <TelepathyQt/PendingOperation>
#include <TelepathyQt/PendingVariant>
#include <TelepathyQt/Constants>

namespace Tp
{
class DBusProxy;
}

class TELEPATHY_TESTLIB_EXPORT Test : public QObject
{
    Q_OBJECT

public:

    Test(QObject *parent = 0);

    virtual ~Test();

    QEventLoop *mLoop;
    void processDBusQueue(Tp::DBusProxy *proxy);

protected:
    template<typename T> bool waitForProperty(Tp::PendingVariant *pv, T *value);

protected Q_SLOTS:
    void expectSuccessfulCall(QDBusPendingCallWatcher*);
    void expectSuccessfulCall(Tp::PendingOperation*);
    void expectFailure(Tp::PendingOperation*);
    void expectSuccessfulProperty(Tp::PendingOperation *op);
    void onWatchdog();

    virtual void initTestCaseImpl();
    virtual void initImpl();

    virtual void cleanupImpl();
    virtual void cleanupTestCaseImpl();

private:
    // The property retrieved by expectSuccessfulProperty()
    QVariant mPropertyValue;
};

template<typename T>
bool Test::waitForProperty(Tp::PendingVariant *pv, T *value)
{
    connect(pv,
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(expectSuccessfulProperty(Tp::PendingOperation*)));
    if (mLoop->exec() == 0) {
        *value = qdbus_cast<T>(mPropertyValue);
        return true;
    }
    else {
        *value = T();
        return false;
    }
}

#endif
