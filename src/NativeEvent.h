/******************************************************************************
    nativeevent.h: description
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

#ifndef NATIVEEVENT_H
#define NATIVEEVENT_H

#include "nativeevent_global.h"

class Q_EXPORT NativeEvent
{
public:
    enum Type {
        Windows,
        X11,
        Mac,
        Unknow
    };

    NativeEvent();
    virtual ~NativeEvent();

    bool isListening() const;
    void startListen();
    void stopListen();

    //stop next handler if return true
    virtual bool handle(Type eventType, void *message, long *result) = 0;
private:
    bool mIsListening;
};

#endif //NATIVEEVENT_H

