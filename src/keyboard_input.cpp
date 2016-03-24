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

void KeyboardInput::onSDLEvent(SDL_Event event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_h:
                emitFunction("l_");
                break;
            case SDLK_j:
                emitFunction("d_");
                break;
            case SDLK_k:
                emitFunction("u_");
                break;
            case SDLK_l:
                emitFunction("r_");
                break;
            case SDLK_y:
                emitFunction("ul");
                break;
            case SDLK_u:
                emitFunction("ur");
                break;
            case SDLK_b:
                emitFunction("dl");
                break;
            case SDLK_n:
                emitFunction("dr");
                break;
        }
    }
}