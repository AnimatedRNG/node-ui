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
    
    bool only_dominant = (*(Config::root))["only_dominant_hand"].asBool();
    bool right_handed = (*(Config::root))["right_handed"].asBool();
    float threshold = (*(Config::root))["gesture_threshold_velocity"].asFloat();
    float zThresh = (*(Config::root))["z_threshold_velocity"].asFloat();
    int regainThresh =
        (*(Config::root))["gesture_threshold_velocity"].asInt();
    int actionThresh =
        (*(Config::root))["action_delay"].asInt();
        
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

void LeapInput::onKeyEvent(QKeyEvent* event) {

}

void LeapInput::onFocusChange(const bool& hasFocus) {
    // Set the focus and put a delay down
    listener.focus = hasFocus;
    listener.delayTimestamp = LeapListener::timestamp();
}