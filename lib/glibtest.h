/* Base class for Telepathy-Qt4 based unit tests
 *
 * Copyright (C) 2009 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright (C) 2009 Nokia Corporation
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

#ifndef TP_GLIBTEST_H
#define TP_GLIBTEST_H

#include "test.h"

namespace Tp {

class TELEPATHY_QT4_EXPORT GlibTest : public Tp::Test
{
    Q_OBJECT

public:

    GlibTest(QObject* parent = 0);
    virtual ~GlibTest();

    void setupExampleConnection(const QString &account, const QString &accountId,
                                const QString &protocol, const QString &contactList);
    QPair< QString, QString > exampleConnectionData() const;

protected Q_SLOTS:
    virtual void cleanupTestCaseImpl();

private:
    class Private;
    Private * const d;
};

}

#endif // TP_GLIBTEST_H