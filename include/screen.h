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

#include "SDL.h"
#include "SDL_shape.h"

#include "util.h"
#include "nodesprite.h"

class Screen {
  public:
    Screen(std::function<void(SDL_Event)> controller);
    Screen(Screen&&) =
        default;                                                                            // Move constructor
    ~Screen();                                                                              // Destructor
    Screen& operator= (const Screen&)& =
        default;                                                                            // Copy assignment operator
    Screen& operator= (Screen&&)& =
        default;                                                                            // Move assignment operator
        
    void start();
    static void terminate();
    
    std::pair<int, int> getResolution();
    
    static constexpr const char* WINDOW_NAME = "NodeUI";
    static constexpr int FRAMERATE = 60;
    
    static constexpr int HORIZONTAL_NODE_NUM = 3;
    static constexpr int VERTICAL_NODE_NUM = 3;
    
    static bool initialized;
  private:
    void render();
    
    util::WindowProperties properties;
    std::function<void(SDL_Event)> controller;
    int width, height;
    
    std::vector<NodeSprite> nodesprites;
};