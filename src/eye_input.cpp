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

EyeTracker::EyeTracker() {
    stopEyeTracking.test_and_set();
    cv::namedWindow("eye_view");
    cv::namedWindow("left_eye");
    cv::namedWindow("right_eye");
    cv::namedWindow("sample_eye_gradient_x");
    cv::namedWindow("sample_eye_gradient_y");
    cv::namedWindow("sample_eye_gradient_mag");
    
    if (!face_cascade.load("assets/tracking/lbpcascade_frontalface.xml")) {
        throw std::runtime_error("Failed to load LBP face classifier!");
    }
    
    if (!eye_cascade.load(
                "assets/tracking/haarcascade_eye_tree_eyeglasses.xml")) {
        throw std::runtime_error("Failed to load Haar eye classifier!");
    }
}

void EyeTracker::eyeTracking(std::function<void(std::string)> emitter) {
    cv::VideoCapture capture;
    cv::Mat frame;
    cv::Mat frame_gray;
    capture.open(-1);
    if (!capture.isOpened()) {
        DEBUG("Error opening video capture!");
        stopEyeTracking.test_and_set();
        return;
    }
    auto lastTimestamp = util::timestamp();
    std::vector<cv::Rect> faces;
    std::vector<cv::Rect> eyes;
    std::pair<cv::Rect, cv::Rect> eyeRects;
    
    while (stopEyeTracking.test_and_set() && capture.read(frame)) {
        if (frame.empty()) {
            DEBUG("No captured frame!");
            break;
        }
        
        cv::flip(frame, frame, 1);
        
        cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(frame_gray, frame_gray);
        
        eye_cascade.detectMultiScale(frame_gray, eyes,
                                     1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE,
                                     cv::Size(30, 30));
                                     
        for (cv::Rect eye : eyes) {
            cv::Point eyeCenter(eye.x + eye.width / 2,
                                eye.y + eye.height / 2);
            cv::circle(frame, eyeCenter,
                       cvRound((eye.width + eye.height) * 0.25),
                       cv::Scalar(255, 0, 255), 3, 8, 0);
        }
        
        if (eyes.size() == 2 && (eyes[0] & eyes[1]).area() == 0) {
            if (eyes[0].x < eyes[1].x)
                eyeRects = std::make_pair(eyes[0], eyes[1]);
            else
                eyeRects = std::make_pair(eyes[1], eyes[0]);
        }
        
        cv::imshow("eye_view", frame);
        if (eyeRects.first.area() != 0 && eyeRects.second.area() != 0) {
            cv::Mat left_eye = frame_gray(eyeRects.first);
            cv::Mat right_eye = frame_gray(eyeRects.second);
            resizeAndRender(left_eye, "left_eye");
            resizeAndRender(right_eye, "right_eye");
            computePupilLocation(left_eye);
        }
        DEBUG("Elapsed milliseconds: " << util::timestamp() - lastTimestamp);
        lastTimestamp = util::timestamp();
    }
    capture.release();
    DEBUG("NOT EYE TRACKING");
}

cv::Point EyeTracker::computePupilLocation(cv::Mat eye) {
    cv::Mat x_gradient = computeMaxGradient(eye);
    cv::Mat y_gradient = computeMaxGradient(eye.t()).t();
    resizeAndRender(x_gradient, "sample_eye_gradient_x");
    resizeAndRender(y_gradient, "sample_eye_gradient_y");
    cv::Mat magnitude = computeMagnitudes(x_gradient, y_gradient);
    resizeAndRender(magnitude, "sample_eye_gradient_mag");
}

cv::Mat EyeTracker::computeMaxGradient(cv::Mat eye) {
    cv::Mat gradient(eye.rows, eye.cols, CV_64F);
    for (int y = 0; y < eye.rows; ++y) {
        const uchar* eye_row = eye.ptr<uchar>(y);
        double* gradient_row = gradient.ptr<double>(y);
        for (int x = 1; x < eye.cols - 1; ++x) {
            gradient_row[x] = (eye_row[x + 1] - eye_row[x - 1]) / 2.0;
        }
        gradient_row[0] = eye_row[1] - eye_row[0];
        gradient_row[eye.cols - 1] =
            eye_row[eye.cols - 1] - eye_row[eye.cols - 2];
    }
    
    return gradient;
}

cv::Mat EyeTracker::computeMagnitudes(cv::Mat mat1, cv::Mat mat2) {
    cv::Mat magnitudes(mat1.rows, mat2.cols, CV_64F);
    for (int y = 0; y < mat1.rows; ++y) {
        const double* mat1_row = mat1.ptr<double>(y);
        const double* mat2_row = mat2.ptr<double>(y);
        double* mags_row = magnitudes.ptr<double>(y);
        for (int x = 0; x < mat1.cols; ++x) {
            const double gX = mat1_row[x];
            const double gY = mat2_row[x];
            double magnitude = sqrt((gX * gX) + (gY * gY));
            mags_row[x] = magnitude;
        }
    }
    return magnitudes;
}

void EyeTracker::resizeAndRender(cv::Mat image, const std::string& name) {
    cv::Mat image_resized;
    cv::resize(image, image_resized, cv::Size(200, 200));
    cv::imshow(name, image_resized);
}
