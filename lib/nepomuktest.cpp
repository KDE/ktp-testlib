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


#include "nepomuktest.h"
#include <Nepomuk/ResourceManager>

namespace Tp {

NepomukTest::NepomukTest(QObject* parent)
    : GLibTest(parent)
{
}

NepomukTest::~NepomukTest()
{

}

void NepomukTest::cleanupTestCaseImpl()
{
    Tp::GLibTest::cleanupTestCaseImpl();
}

void NepomukTest::initTestCaseImpl()
{
    Tp::Test::initTestCaseImpl();

    Nepomuk::ResourceManager::instance()->init();
}

void NepomukTest::initImpl()
{
    Tp::Test::initImpl();
}

void NepomukTest::cleanupImpl()
{
    Tp::Test::cleanupImpl();
}

}
