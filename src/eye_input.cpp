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
    cv::namedWindow("sample_eye_weight");
    cv::namedWindow("sample_eye_possible_centers");
    cv::namedWindow("sample_eye_mask");
    
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
                                     
        auto crop_eye = [](cv::Rect eye) {
            return cv::Rect(eye.x + eye.width / 6,
                            eye.y + eye.height / 4,
                            (2 * eye.width) / 3,
                            (1 * eye.height) / 2);
        };
        for (cv::Rect eye : eyes) {
            cv::Point eyeCenter(eye.x + eye.width / 2,
                                eye.y + eye.height / 2);
            cv::circle(frame, eyeCenter,
                       cvRound((eye.width + eye.height) * 0.25),
                       cv::Scalar(255, 0, 255), 3, 8, 0);
        }
        
        if (eyes.size() == 2 && (eyes[0] & eyes[1]).area() == 0) {
            if (eyes[0].x < eyes[1].x)
                eyeRects = std::make_pair(crop_eye(eyes[0]),
                                          crop_eye(eyes[1]));
            else
                eyeRects = std::make_pair(crop_eye(eyes[1]),
                                          crop_eye(eyes[0]));
        }
        
        cv::imshow("eye_view", frame);
        if (eyeRects.first.area() != 0 && eyeRects.second.area() != 0) {
            cv::Mat left_eye = frame_gray(eyeRects.first);
            cv::Mat right_eye = frame_gray(eyeRects.second);
            cv::Point left_eye_loc =
                computePupilLocation(resizeIdeal(left_eye));
            cv::Point right_eye_loc =
                computePupilLocation(resizeIdeal(right_eye));
            cv::circle(left_eye, left_eye_loc,
                       2,
                       cv::Scalar(255, 0, 255), 1, 8, 0);
            cv::circle(right_eye, right_eye_loc,
                       2,
                       cv::Scalar(255, 0, 255), 1, 8, 0);
            resizeAndRender(left_eye, "left_eye");
            resizeAndRender(right_eye, "right_eye");
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
    cv::Mat magnitude = computeMagnitudes(x_gradient, y_gradient);
    double gradient_threshold =
        computeDynamicThreshold(magnitude, 50.0);
    resizeAndRender(magnitude, "sample_eye_gradient_mag");
    DEBUG("Gradient threshold: " << gradient_threshold);
    
    for (int y = 0; y < eye.rows; ++y) {
        double* x_row = x_gradient.ptr<double>(y);
        double* y_row = y_gradient.ptr<double>(y);
        const double* mag_row = magnitude.ptr<double>(y);
        for (int x = 0; x < eye.cols; ++x) {
            double gX = x_row[x];
            double gY = y_row[x];
            double m = mag_row[x];
            if (m > gradient_threshold) {
                x_row[x] = gX / m;
                y_row[x] = gY / m;
            } else {
                x_row[x] = 0;
                y_row[x] = 0;
            }
        }
    }
    
    resizeAndRender(x_gradient, "sample_eye_gradient_x");
    resizeAndRender(y_gradient, "sample_eye_gradient_y");
    
    cv::Mat weight;
    cv::GaussianBlur(eye, weight,
                     cv::Size(5, 5), 0, 0);
    for (int y = 0; y < weight.rows; ++y) {
        unsigned char* row = weight.ptr<unsigned char>(y);
        for (int x = 0; x < weight.cols; ++x) {
            row[x] = (255 - row[x]);
        }
    }
    
    resizeAndRender(weight, "sample_eye_weight");
    
    cv::Mat out_sum = cv::Mat::zeros(eye.rows, eye.cols, CV_64F);
    for (int y = 0; y < weight.rows; ++y) {
        const double* Xr = x_gradient.ptr<double>(y);
        const double* Yr = y_gradient.ptr<double>(y);
        for (int x = 0; x < weight.cols; ++x) {
            double gX = Xr[x];
            double gY = Yr[x];
            if (gX == 0.0 && gY == 0.0) {
                continue;
            }
            test_center(x, y, weight, gX, gY, out_sum);
        }
    }
    
    double gradients_num = weight.rows * weight.cols;
    cv::Mat out;
    out_sum.convertTo(out, CV_32F, 1.0 / gradients_num);
    
    cv::Point max_point;
    double max_value;
    cv::minMaxLoc(out, NULL, &max_value, NULL, &max_point);
    
    cv::Mat flood_clone;
    double flood_thresh = max_value * 0.97;
    cv::threshold(out, flood_clone, flood_thresh, 0.0f, cv::THRESH_TOZERO);
    cv::Mat mask = floodKillEdges(flood_clone);
    
    cv::minMaxLoc(out, NULL, &max_value, NULL, &max_point, mask);
    
    resizeAndRender(mask, "sample_eye_mask");
    resizeAndRender(out, "sample_eye_possible_centers");
    return max_point;
}

void EyeTracker::test_center(const int& x, const int& y,
                             cv::Mat weight, double gX, double gY,
                             cv::Mat& out) {
    for (int cy = 0; cy < out.rows; ++cy) {
        double* out_row = out.ptr<double>(cy);
        const unsigned char* weight_row =
            weight.ptr<unsigned char>(cy);
        for (int cx = 0; cx < out.cols; ++cx) {
            if (x == cx && y == cy) {
                continue;
            }
            
            double dx = x - cx;
            double dy = y - cy;
            
            double magnitude = sqrt((dx * dx) + (dy * dy));
            dx = dx / magnitude;
            dy = dy / magnitude;
            
            double dot_product = dx * gX + dy * gY;
            dot_product = std::max(0.0, dot_product);
            
            out_row[cx] = dot_product * dot_product * weight_row[cx];
        }
    }
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

double EyeTracker::computeDynamicThreshold(cv::Mat mat, double std_dev_k) {
    cv::Scalar grad_std_dev, grad_mean;
    cv::meanStdDev(mat, grad_mean, grad_std_dev);
    double deviation = grad_std_dev[0] / sqrt(mat.rows * mat.cols);
    return std_dev_k * deviation + grad_mean[0];
}

cv::Mat EyeTracker::floodKillEdges(cv::Mat& mat) {
    cv::rectangle(mat, cv::Rect(0, 0, mat.cols, mat.rows), 255);
    cv::Mat mask(mat.rows, mat.cols, CV_8U, 255);
    std::queue<cv::Point> to_do;
    to_do.push(cv::Point(0, 0));
    
    auto should_push = [&](cv::Point p) {
        return p.x >= 0 && p.x < mat.cols && p.y >= 0 && p.y < mat.rows;
    };
    
    while (!to_do.empty()) {
        cv::Point p = to_do.front();
        to_do.pop();
        if (mat.at<float>(p) == 0.0f) {
            continue;
        }
        
        // right
        cv::Point np(p.x + 1, p.y);
        if (should_push(np))
            to_do.push(np);
            
        // left
        np.x = p.x - 1;
        np.y = p.y;
        if (should_push(np))
            to_do.push(np);
            
        // down
        np.x = p.x;
        np.y = p.y - 1;
        if (should_push(np))
            to_do.push(np);
            
        // up
        np.x = p.x;
        np.y = p.y + 1;
        if (should_push(np))
            to_do.push(np);
            
        mat.at<float>(p) = 0.0f;
        mask.at<uchar>(p) = 0;
    }
    return mask;
}

cv::Mat EyeTracker::resizeIdeal(cv::Mat image) {
    cv::Mat image_resized;
    double aspect_ratio = ((double) image.rows) / ((double) image.cols);
    cv::resize(image, image_resized, cv::Size(40, 40 * aspect_ratio));
    return image_resized;
}

void EyeTracker::resizeAndRender(cv::Mat image, const std::string& name) {
    cv::Mat image_resized;
    double aspect_ratio = ((double) image.rows) / ((double) image.cols);
    cv::resize(image, image_resized, cv::Size(200, 200 * aspect_ratio));
    cv::imshow(name, image_resized);
}
