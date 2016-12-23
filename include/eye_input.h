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
#include <atomic>
#include <iostream>

#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include "util.h"
#include "input_device.h"
#include "config.h"

class EyeTracker {
  public:
    static std::atomic_flag stopEyeTracking;
    
    EyeTracker() {
        stopEyeTracking.test_and_set();
    }
    
    void stopTracking();
    
    void eyeTracking(std::function<void(std::string)> emitter);
};

class EyeInput : public InputDevice {
  public:
    EyeInput(std::function<void(std::string)> emitter):
        InputDevice(emitter),
        tracker() { }
        
    virtual void onKeyEvent(QKeyEvent* event);
    void onFocusChange(const bool& hasFocus);
  private:
    EyeTracker tracker;
};

