// Copyright (C) 2016 by Srinivas Kaza <srinivas@kaza.io>

// This file is part of NodeUI

// NodeUI free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.

// NodeUI is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.

// You should have received a copy of the GNU General Public License along
// with NodeUI.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <thread>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "util.h"
#include "controller.h"

#define SUPER_LEFT 0xffeb
#define CTRL_MASK 4
#define SUPER_MASK 64

namespace HotKey {

    Display* dpy;
    Window root;
    int keycode;
    int modifiers;
    bool die = false;
    
    void configureHotkey(int keysym, int mods,
                         std::function<void(Controller*)> callback,
                         Controller* controller) {
        dpy = XOpenDisplay(0);
        root = DefaultRootWindow(dpy);
        keycode = XKeysymToKeycode(dpy, keysym);
        modifiers = mods;
        
        XGrabKey(dpy,
                 keycode,
                 modifiers,
                 root,
                 False,
                 GrabModeAsync,
                 GrabModeAsync);
        XSelectInput(dpy, root, KeyPressMask);
        auto inputThread = [&](std::function<void(Controller*)> func) {
            XEvent ev;
            while (!die) {
                XNextEvent(dpy, &ev);
                switch (ev.type) {
                    case KeyPress:
                        func(controller);
                        break;
                }
            }
        };
        std::thread hotkeyListener(inputThread, callback);
        hotkeyListener.detach();
    }
    
    void dispose() {
        die = true;
        XUngrabKey(dpy, keycode, modifiers, root);
        XCloseDisplay(dpy);
    }
}
