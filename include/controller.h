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

#include <QKeyEvent>

#include "util.h"
#include "node.h"
#include "model.h"
#include "screen.h"
#include "input_device.h"
#include "keyboard_input.h"

void onReceive(std::string str, void* controller);

class Controller {
  public:
    Controller(std::shared_ptr<Model> model, std::shared_ptr<Screen> screen) :
        inputDevices() {
        this->model = model;
        this->screen = screen;
        
        auto receiveFunc = [&](std::string str) {
            return onReceive(str, this);
        };
        std::function<void(std::string)> func = receiveFunc;
        inputDevices.push_back(std::shared_ptr<InputDevice>(new KeyboardInput(
                                   func)));
                                   
        auto signalAll = [&](QKeyEvent * event) {
            for (auto device : this->inputDevices)
                device->onKeyEvent(event);
        };
        
        this->screen->setController(signalAll);
        this->loadIcons();
        this->updateView();
    }
    
    void updateView();
    
    friend void onReceive(std::string str, Controller* controller);
  private:
    void loadIcons();
    
    std::shared_ptr<Model> model;
    std::shared_ptr<Screen> screen;
    
    std::vector<std::shared_ptr<InputDevice>> inputDevices;
};