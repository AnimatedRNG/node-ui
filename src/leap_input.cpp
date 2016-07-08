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

#include "leap_input.h"

void LeapListener::onConnect(const Leap::Controller& controller) {
    DEBUG("Connected to Leap!");
}

void LeapListener::onDisconnect(const Leap::Controller& controller) {
    DEBUG("Disconnected from Leap!");
}

void LeapListener::onFrame(const Leap::Controller& controller) {
    const Leap::Frame frame = controller.frame();
    
    const bool only_dominant =
        (*(Config::root))["only_dominant_hand"].asBool();
    const bool right_handed =
        (*(Config::root))["right_handed"].asBool();
    const bool position_mode =
        (*(Config::root))["position_mode"].asBool();
        
    // I've never actually checked how many hands
    // the Leap Motion can detect. So this implementation
    // assumes that the user has an integral number of hands.
    Leap::HandList hands = frame.hands();
    Leap::Hand hand;
    
    // If they have no hands, we can't do anything
    if (hands.count() == 0) {
        // If the window was focused and the user actually
        // had their hand in view
        if (focus && hadHand)
            emitFunction("EXIT");
    } else {
        if (only_dominant) {
            // If the user only wants their dominant hand
            // to be detected, find all their dominant hands
            // (if they have more than two hands)
            for (int h_c = 0; h_c < hands.count(); h_c++) {
                if (right_handed ^ hands[h_c].isLeft()) {
                    hand = hands[h_c];
                    break;
                }
            }
        }
        // Otherwise we just want the first hand that the
        // sensor can see
        else {
            hand = hands[0];
        }
    }
    
    bool isFist = false;
    // If they don't actually have a hand out,
    // we set the had hand flag so that we don't keep
    // triggering the EXIT signal
    if (!hand.isValid()) {
        hadHand = false;
        hadRegainedFocus = false;
        return;
    } else {
        // If there's no visible fingers, the user is making a fist
        isFist = hand.pointables().extended().count() == 0;
        // If they have a fist and the window has focus, exit
        if (isFist && focus)
            emitFunction("EXIT");
        // If we don't have focus, but we didn't have a hand there
        // before and now we do (and we know it's not a fist),
        // then show the window
        if (!focus && !hadHand && !isFist) {
            emitFunction("SHOW");
            delayTimestamp = timestamp();
        }
        // Mark the hand as visible
        hadHand = true;
    }
    
    if (position_mode)
        handleHandPosition(hand);
    else
        handleHandVelocity(hand);
}

void LeapListener::handleHandVelocity(const Leap::Hand& hand) {
    const float threshold =
        (*(Config::root))["gesture_threshold_velocity"].asFloat();
    const float zThresh =
        (*(Config::root))["z_threshold_velocity"].asFloat();
    const int regainThresh =
        (*(Config::root))["regain_focus_velocity"].asInt();
    const int actionThresh =
        (*(Config::root))["action_delay"].asInt();
        
    bool isFist = hand.pointables().extended().count() == 0;
    
    Leap::Vector palmVelocity = hand.palmVelocity();
    float zMovement = palmVelocity.z;
    palmVelocity.z = 0.0;
    
    // If the window has been open for long enough and
    // we haven't done anything in the action threshold
    bool regainFocus = ((timestamp() - delayTimestamp) > regainThresh)
                       && ((timestamp() - actionTimestamp) > actionThresh)
                       && !isFist;
                       
    // If the hand is moving fast enough
    if (palmVelocity.magnitude() > threshold && zMovement < zThresh) {
        // Get the hand angle
        float angle = atan2(palmVelocity.y, palmVelocity.x);
        angle = (angle > 0 ? angle : (2 * M_PI + angle)) * 360 / (2 * M_PI);
        
        // Mark the time so that we ignore actions within
        // too close a margin
        actionTimestamp = timestamp();
        
        // If we should regain focus, check events
        if (regainFocus) {
            std::string gesture = getPose(hand);
            if (gesture != NOTHING)
                emitFunction((*(Config::root))[gesture].asString());
            if (checkEpsilon(angle, 180))
                emitFunction("l_");
            else if (checkEpsilon(angle, 270))
                emitFunction("d_");
            else if (checkEpsilon(angle, 90))
                emitFunction("u_");
            else if (checkEpsilon(angle, 0))
                emitFunction("r_");
            else if (checkEpsilon(angle, 135))
                emitFunction("ul");
            else if (checkEpsilon(angle, 225))
                emitFunction("dl");
            else if (checkEpsilon(angle, 45))
                emitFunction("ur");
            else if (checkEpsilon(angle, 315))
                emitFunction("dr");
        }
    }
}

void LeapListener::handleHandPosition(const Leap::Hand& hand) {
    const int regainThresh =
        (*(Config::root))["regain_focus_threshold"].asInt();
    const int actionThresh =
        (*(Config::root))["action_delay"].asInt();
    const float gridSize =
        (*(Config::root))["grid_size"].asFloat();
        
    bool isFist = hand.pointables().extended().count() == 0;
    
    // If the window has been open for long enough and
    // we haven't done anything in the action threshold
    bool regainFocus = ((timestamp() - delayTimestamp) > regainThresh)
                       && ((timestamp() - actionTimestamp) > actionThresh)
                       && !isFist;
                       
    if (regainFocus) {
        // If the hand just became visible, make the palm
        // position the relative center
        if (!hadRegainedFocus) {
            relativeCenter = hand.stabilizedPalmPosition();
            relativeCenter.z = 0;
            hadRegainedFocus = true;
            lastPosition = std::make_pair(0, 0);
            actionTimestamp = timestamp();
        } else {
            Leap::Vector currentPosition = hand.stabilizedPalmPosition();
            currentPosition.z = 0;
            
            // Figure out how much the hand has moved from the
            // relative center
            Leap::Vector diff = currentPosition - relativeCenter;
            int dx = ((int) diff.x / gridSize);
            int dy = ((int) diff.y / gridSize);
            
            // Normalize to -1, 0, 1
            dx = (dx != 0) ? dx / abs(dx) : 0;
            dy = (dy != 0) ? dy / abs(dy) : 0;
            
            // Get the difference between this delta and the last delta
            const std::pair<int, int> n_diff(dx - lastPosition.first,
                                             dy - lastPosition.second);
                                             
            std::string gesture = getPose(hand);
            Json::Value poses = (*(Config::root))["poses"];
            if (gesture != NOTHING) {
                if (gesture == "BACK") {
                    relativeCenter = currentPosition;
                }
                emitFunction(poses[gesture].asString());
            } else if (n_diff == std::make_pair(0, 0))
                return;
            else if (n_diff == std::make_pair(-1, 0))
                emitFunction("l_");
            else if (n_diff == std::make_pair(0, -1))
                emitFunction("d_");
            else if (n_diff == std::make_pair(0, 1))
                emitFunction("u_");
            else if (n_diff == std::make_pair(1, 0))
                emitFunction("r_");
            else if (n_diff == std::make_pair(-1, 1))
                emitFunction("ul");
            else if (n_diff == std::make_pair(-1, -1))
                emitFunction("dl");
            else if (n_diff == std::make_pair(1, 1))
                emitFunction("ur");
            else if (n_diff == std::make_pair(1, -1))
                emitFunction("dr");
                
            // Save this delta so we don't repeat the action
            lastPosition = std::make_pair(dx, dy);
            actionTimestamp = timestamp();
        }
    }
}

std::string LeapListener::getPose(const Leap::Hand& hand) {
    const float pinchThresh =
        (*(Config::root))["pinch_threshold"].asFloat();
    if (hand.pinchStrength() > pinchThresh)
        return PINCH;
    else
        return NOTHING;
}

void LeapInput::onKeyEvent(QKeyEvent* event) {

}

void LeapInput::onFocusChange(const bool& hasFocus) {
    // Set the focus and put a delay down
    listener.focus = hasFocus;
    listener.delayTimestamp = LeapListener::timestamp();
}
