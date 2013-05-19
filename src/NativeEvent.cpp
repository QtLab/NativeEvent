/******************************************************************************
    nativeevent.cpp: description
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
#include <algorithm>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <windows.h>
#include <windowsx.h>

using namespace std;

static list<NativeEvent*> sNativeEventListeners;

void createWindowsMsgWindow();
void destroyWindowsMsgWindow();

NativeEvent::NativeEvent():
    mIsListening(false)
{
}

NativeEvent::~NativeEvent()
{
    stopListen();
}

bool NativeEvent::isListening() const
{
    return mIsListening;
}

void NativeEvent::startListen()
{
    cout << __FUNCTION__ << endl;
    if (isListening())
        return;
    mIsListening = true;
    sNativeEventListeners.push_front(this);
    cout << "sNativeEventListeners.size = " << sNativeEventListeners.size()
         << ", first: " << (*sNativeEventListeners.begin())
         << "==" << sNativeEventListeners.front() << endl;

#ifdef Q_OS_WIN
    createWindowsMsgWindow();
#endif //Q_OS_WIN
}

void NativeEvent::stopListen()
{
    cout << __FUNCTION__ << endl;
    if (!isListening())
        return;
    mIsListening = false;
    list<NativeEvent*>::iterator it = std::find(sNativeEventListeners.begin(), sNativeEventListeners.end(), this);
    if (it != sNativeEventListeners.end())
        sNativeEventListeners.erase(it);
#ifdef Q_OS_WIN
    if (sNativeEventListeners.empty())
        destroyWindowsMsgWindow();
#endif //Q_OS_WIN

}


#ifdef Q_OS_WIN
#define USE_EX 0
static list<string> sRegisteredWindowClassNames;

static bool windowsProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
    *result = 0;
    if (sNativeEventListeners.empty())
        return false;
    MSG msg;
    msg.hwnd = hwnd;         // re-create MSG structure
    msg.message = message;   // time and pt fields ignored
    msg.wParam = wParam;
    msg.lParam = lParam;
    msg.pt.x = GET_X_LPARAM(lParam);
    msg.pt.y = GET_Y_LPARAM(lParam);

    // Run the native event filters.
    long filterResult = 0;
    cout << "sNativeEventListeners.size = " << sNativeEventListeners.size() << endl;
    for (list<NativeEvent*>::iterator it = sNativeEventListeners.begin(); it != sNativeEventListeners.end(); ++it) {
        cout << "native event handler: " << std::hex << (long)(*it) << std::dec << endl;
        if (!(*it))
            continue;
        if ((*it)->handle(NativeEvent::Windows, &msg, &filterResult)) {
            *result = LRESULT(filterResult);
            return true;
        }
    }
    return false;
}

std::string registerWindowClass(std::string cname, WNDPROC proc, unsigned style = 0, HBRUSH brush = 0, bool icon = false)
{
    cout << "register window class: " << cname << endl;
    static int classExists = -1;
    const HINSTANCE appInstance = (HINSTANCE)GetModuleHandle(0);
    if (classExists == -1) {
        WNDCLASS wcinfo;
        classExists = GetClassInfo(appInstance, (wchar_t*)cname.c_str(), &wcinfo);
        classExists = classExists && wcinfo.lpfnWndProc != proc;
    }

    if (classExists) {
        stringstream ss;
        ss << (unsigned)proc;
        cname += ss.str();
    }
    list<string>::iterator it = std::find(sRegisteredWindowClassNames.begin(), sRegisteredWindowClassNames.end(), cname);
    if (it != sRegisteredWindowClassNames.end()) {       // already registered in our list
        cout << "already registered in our list" << endl;
        return cname;
    }
    cout << "setup wc" << endl;
#if !defined(Q_OS_WINCE) && USE_EX
    WNDCLASSEX wc;
    wc.cbSize       = sizeof(WNDCLASSEX);
#else
    WNDCLASS wc;
#endif
    wc.style        = style;
    wc.lpfnWndProc  = proc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = 0;
    wc.hInstance    = appInstance;
    wc.hCursor      = 0;
    wc.hbrBackground = brush; //TODO: QtBUG? qt not set this for wince. on winnt, it will cause RegisterClass fail
#if !defined(Q_OS_WINCE) && USE_EX
    if (icon) {
        wc.hIcon = (HICON)LoadImage(appInstance, L"IDI_ICON1", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
        if (wc.hIcon) {
            int sw = GetSystemMetrics(SM_CXSMICON);
            int sh = GetSystemMetrics(SM_CYSMICON);
            wc.hIconSm = (HICON)LoadImage(appInstance, L"IDI_ICON1", IMAGE_ICON, sw, sh, 0);
        } else {
            wc.hIcon = (HICON)LoadImage(0, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
            wc.hIconSm = 0;
        }
    } else {
        wc.hIcon    = 0;
        wc.hIconSm  = 0;
    }
#else
    if (icon) {
        wc.hIcon = (HICON)LoadImage(appInstance, L"IDI_ICON1", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
    } else {
        wc.hIcon    = 0;
    }
#endif

    wc.lpszMenuName  = 0;
    wc.lpszClassName = (wchar_t*)cname.c_str(); //to utf16
    cout << "register class ex" << endl;
#if !defined(Q_OS_WINCE) && USE_EX
    ATOM atom = RegisterClassEx(&wc); //TODO: RegisterClassEx
#else
    ATOM atom = RegisterClass(&wc);
#endif

    if (!atom)
        std::cerr << "Failed to Register window class: " << cname << endl;

    sRegisteredWindowClassNames.push_back(cname);
    std::cout << std::noskipws << __FUNCTION__ << ' ' << cname
              << " style=" << std::showbase << std::hex << style
              << " brush=" << brush << " icon=" << icon << " atom=" << atom << endl;
    return cname;
}

static void unregisterWindowClasses()
{
    const HINSTANCE appInstance = (HINSTANCE)GetModuleHandle(0);
    for (list<string>::const_iterator cit = sRegisteredWindowClassNames.begin(); cit != sRegisteredWindowClassNames.end(); ++cit) {
        cout << __FUNCTION__ << *cit;
        UnregisterClass((wchar_t*)(*cit).c_str()/*TODO: to utf16*/, appInstance);
    }
    sRegisteredWindowClassNames.clear();
}

/*!
    \brief Convenience to create a non-visible, message-only dummy
    window for example used as clipboard watcher or for GL.
*/
#ifndef HWND_MESSAGE
#define HWND_MESSAGE ((HWND)-3)
#endif //HWND_MESSAGE
static HWND createDummyWindow(const std::string &classNameIn, const wchar_t *windowName, WNDPROC wndProc = 0, DWORD style = WS_OVERLAPPED)
{
    cout << "create dummy window" << endl;
    if (!wndProc)
        wndProc = DefWindowProc;
    string className = registerWindowClass(classNameIn, wndProc);
    //TODO: both CreateWindow(Ex) are OK. (Ex for wince too)
    return CreateWindowEx(0, (wchar_t*)className.c_str()/*TODO: to utf16*/,
                        windowName, style,
                        CW_USEDEFAULT, CW_USEDEFAULT, //TODO:CW_USEDEFAULT
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        0, /*TODO: qt is HWND_MESSAGE, not work here*/
                        NULL, (HINSTANCE)GetModuleHandle(0), NULL);
}


LRESULT CALLBACK WindowsWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result;
    const bool handled = windowsProc(hwnd, message, wParam, lParam, &result);
    if (!handled)
        result = DefWindowProc(hwnd, message, wParam, lParam);
    return result;
}

static HWND MsgWindow = 0;

void createWindowsMsgWindow()
{
    static bool sMsgWindowCreated = false;
    if (sMsgWindowCreated) {
        cout << "msg window exists" << endl;
        return;
    }
    cout << "create msg window" << endl;
    MsgWindow = createDummyWindow("NativeEventMsgWindow", L"NativeEventMsgWindow", WindowsWndProc);
    sMsgWindowCreated = true;
}

void destroyWindowsMsgWindow()
{
    DestroyWindow(MsgWindow);
}

#endif
