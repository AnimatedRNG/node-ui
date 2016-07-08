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

#include "util.h"
#include "input_device.h"
#include "model.h"
#include "config.h"

#include <stdexcept>
#include <chrono>

#include "Leap.h"

class LeapListener : public Leap::Listener {
  public:
    LeapListener(std::function<void(std::string)> emitter) :
        emitFunction(emitter),
        focus(false),
        hadHand(false) {};
        
    virtual void onConnect(const Leap::Controller&);
    virtual void onDisconnect(const Leap::Controller&);
    virtual void onFrame(const Leap::Controller&);
    
    inline static uint64_t timestamp() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch()).
               count();
    }
    
    bool focus;
    uint64_t delayTimestamp;
    uint64_t actionTimestamp;
    bool hadHand;
  private:
  
    inline bool checkEpsilon(const float& angle,
                             const float& desired,
                             const float& epsilon = 22.5) {
        if ((desired < 0) || (desired >= 360))
            throw std::out_of_range("Desired angle out of range " +
                                    std::to_string(desired));
                                    
        float lowerBound = desired - epsilon;
        float upperBound = desired + epsilon;
        bool reverse = false;
        
        if (lowerBound < 0) {
            lowerBound += 360;
            reverse = true;
        }
        if (reverse)
            return (angle < upperBound || angle > lowerBound);
        else
            return (angle > lowerBound && angle < upperBound);
    }
    
    std::function<void(std::string)> emitFunction;
};

class LeapInput : public InputDevice {
  public:
    LeapInput(std::function<void(std::string)> emitter):
        InputDevice(emitter),
        controller(),
        listener(emitter) {
        controller.addListener(listener);
    }
    
    ~LeapInput() {
        controller.removeListener(listener);
    }
    
    virtual void onKeyEvent(QKeyEvent* event);
    void onFocusChange(const bool& hasFocus);
    
  private:
    Leap::Controller controller;
    LeapListener listener;
};
