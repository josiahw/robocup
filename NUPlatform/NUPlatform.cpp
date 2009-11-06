/*! @file NUPlatform.cpp
    @brief Partial implementation of base NUPlatform (Robot) class

    @author Jason Kulk
 
 Copyright (c) 2009 Jason Kulk
 
 This file is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This file is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with NUbot.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "NUPlatform.h"

NUPlatform* nuplatform = NULL;

NUPlatform::NUPlatform()
{
    cout << "NUPlatform::NUPlatform" << endl;
    cout << "NUPlatform::NUPlatform. this: " << this << endl;
}

NUPlatform::~NUPlatform()
{
}

void NUPlatform::getName(string& name)
{
    name = m_name;
}

void NUPlatform::getNumber(int& number)
{
    number = m_number;
}

void NUPlatform::test()
{
    cout << "NUPlatform::test()" << endl;
}
