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

#include "keyboard_input.h"

void KeyboardInput::onKeyEvent(QKeyEvent* event) {
    int key = event->key();
    
    switch (key) {
        case Qt::Key_H:
            emitFunction("l_");
            break;
        case Qt::Key_J:
            emitFunction("d_");
            break;
        case Qt::Key_K:
            emitFunction("u_");
            break;
        case Qt::Key_L:
            emitFunction("r_");
            break;
        case Qt::Key_Y:
            emitFunction("ul");
            break;
        case Qt::Key_U:
            emitFunction("ur");
            break;
        case Qt::Key_B:
            emitFunction("dl");
            break;
        case Qt::Key_N:
            emitFunction("dr");
            break;
        case Qt::Key_Backspace:
            emitFunction("BACK");
            break;
        case Qt::Key_Escape:
            emitFunction("EXIT");
            break;
    }
}