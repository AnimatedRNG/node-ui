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

#include "eye_input.h"

std::atomic_flag EyeTracker::stopEyeTracking = ATOMIC_FLAG_INIT;

void EyeInput::onFocusChange(const bool& hasFocus) {
    if (hasFocus) {
        auto runTracker = [](EyeTracker tracker,
        std::function<void(std::string)> emitter) {
            tracker.eyeTracking(emitter);
        };
        std::thread et(runTracker, tracker, emitFunction);
        et.detach();
    } else {
        tracker.stopTracking();
    }
}

void EyeInput::onKeyEvent(QKeyEvent* event) {

}

void EyeTracker::stopTracking() {
    stopEyeTracking.clear();
}

void EyeTracker::eyeTracking(std::function<void(std::string)> emitter) {
    cv::VideoCapture capture;
    cv::Mat frame;
    capture.open(-1);
    if (!capture.isOpened()) {
        DEBUG("Error opening video capture!");
        stopEyeTracking.test_and_set();
        return;
    }
    while (stopEyeTracking.test_and_set() && capture.read(frame)) {
        if (frame.empty()) {
            DEBUG("No captured frame!");
            break;
        }
    }
    DEBUG("NOT EYE TRACKING");
}
