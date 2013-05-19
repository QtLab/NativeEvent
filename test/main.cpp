/******************************************************************************
    tst_NativeEvent: test application for libNativeEvent
    Copyright (C) 2011-2012 Wang Bin <wbsecg1@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
******************************************************************************/

#include "nativeevent.h"
#include <iostream>
#include <windows.h>
#include <winuser.h>

class MyNativeEventListener : public NativeEvent
{
public:
    bool handle(Type eventType, void *message, long *result) {
        MSG *msg = (MSG*)message;
        *result = 0;
        std::cout << "MyNativeEventListener::handle event: " << eventType << ", msg.message" << msg->message << std::endl;
        return false;
    }
};

#include <QCoreApplication>

int main(int argc, char **argv)
{
    MyNativeEventListener tst;
    tst.startListen();
    QCoreApplication app(argc, argv);
    return app.exec();
}

