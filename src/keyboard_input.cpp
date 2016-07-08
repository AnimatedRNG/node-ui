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

#include "keyboard_input.h"

#include "config.h"

void KeyboardInput::onKeyEvent(QKeyEvent* event) {
    int key = event->key();
    
    auto checkIfKeyMatches = [](QKeyEvent * ev, std::string command,
    const Json::Value & keys) {
        const Json::Value listing = keys[command];
        for (int i = 0; i < keys.size(); i++) {
            // Either Qt can create a string representation of the keypress
            // or we need to check it against the key->string map
            if ((ev->text() != QString() &&
                    QString::fromStdString(listing[i].asString()) == ev->text()
                    || (util::keyToString.count(ev->key()) > 0 &&
                        util::keyToString.at(ev->key()) == listing[i].asString())))
                return true;
        }
        return false;
    };
    
    const Json::Value keys = (*(Config::root))["keys"];
    if (checkIfKeyMatches(event, "l_", keys))
        emitFunction("l_");
    else if (checkIfKeyMatches(event, "d_", keys))
        emitFunction("d_");
    else if (checkIfKeyMatches(event, "u_", keys))
        emitFunction("u_");
    else if (checkIfKeyMatches(event, "r_", keys))
        emitFunction("r_");
    else if (checkIfKeyMatches(event, "ul", keys))
        emitFunction("ul");
    else if (checkIfKeyMatches(event, "ur", keys))
        emitFunction("ur");
    else if (checkIfKeyMatches(event, "dl", keys))
        emitFunction("dl");
    else if (checkIfKeyMatches(event, "dr", keys))
        emitFunction("dr");
    else if (checkIfKeyMatches(event, "BACK", keys))
        emitFunction("BACK");
    else if (checkIfKeyMatches(event, "EXIT", keys))
        emitFunction("EXIT");
}

void KeyboardInput::onFocusChange(const bool& hasFocus) {
    
}
