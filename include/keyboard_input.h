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

#include <memory>
#include <unordered_map>
#include <algorithm>
#include <vector>

#include "SDL.h"

#include "input_device.h"
#include "util.h"
#include "model.h"

class KeyboardInput : public InputDevice {
  public:
    KeyboardInput(std::function<void(std::string)> emitter):
        InputDevice(emitter) { }
        
    void onSDLEvent(SDL_Event event);
};