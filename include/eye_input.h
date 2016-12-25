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
#include <queue>
#include <atomic>
#include <iostream>

#include "opencv2/photo.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"

#include "util.h"
#include "input_device.h"
#include "config.h"

class EyeTracker {
  public:
    static std::atomic_flag stopEyeTracking;
    
    explicit EyeTracker();
    
    void stopTracking();
    
    void eyeTracking(std::function<void(std::string)> emitter);
  private:
    cv::Point computePupilLocationHough(cv::Mat eye);
    int getIrisScore(cv::Mat iris);
    cv::Point computePupilLocation(cv::Mat eye);
    void test_center(const int& x, const int& y, cv::Mat weight, double gX,
                     double gY, cv::Mat& out);
    cv::Mat computeMaxGradient(cv::Mat eye);
    cv::Mat computeMagnitudes(cv::Mat mat1, cv::Mat mat2);
    double computeDynamicThreshold(cv::Mat mat, double stdDevFactor);
    cv::Mat floodKillEdges(cv::Mat& mat);
    cv::Mat resizeIdeal(cv::Mat image);
    void resizeAndRender(cv::Mat image, const std::string& name);
    
    cv::CascadeClassifier eye_cascade;
};

template<class T>
class StabilizedMedian {
  public:
    StabilizedMedian(int size) :
        data(),
        size(size) { }
        
    inline void put(T datum) {
        T old_median = datum;
        data.push_back(datum);
        if (data.size() > 1)
            old_median = this->getMedian();
        
        if (data.size() > size) {
            double error = std::abs(datum - old_median) /
                (double) old_median - 1;
            if (error > 0.8 && rand() % 5 < 4) {
                data.erase(data.end() - 1);
                DEBUG("LOTS OF ERROR");
            } else
                data.erase(data.begin());
        }
    }
    
    inline T getMedian() {
        std::vector<T> copy(data.begin(), data.end());
        std::sort(copy.begin(), copy.end());
        return copy[copy.size() / 2];
    }

    inline int getLength() {
        return this->data.size();
    }
    
  private:
    std::vector<T> data;
    int size;
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

