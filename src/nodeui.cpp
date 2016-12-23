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

#include <iostream>

#include <QApplication>

#include "config.h"
#include "screen.h"
#include "model.h"
#include "controller.h"
#include "hotkey.h"


Controller* createUIOverlay() {
    Config::readConfig();
    const Json::Value desktopFiles = (*(Config::root))["desktop_file_dirs"];
    for (int i = 0; i < desktopFiles.size(); i++)
        Config::updateApplicationList(desktopFiles[i].asString().c_str());
    std::shared_ptr<std::vector<std::pair<util::Command, util::vec2i_ptr>>>
    apps = Config::readApplications();
    
    // TODO: Fix odd memory corruption that happens around here on rare occasions
    std::shared_ptr<UIOverlay> screen(new UIOverlay);
    std::shared_ptr<Model> model(new Model(*apps));
    Controller* controller = new Controller(model, screen);
    screen->start();
    return controller;
}

void onHotkeyPress(Controller* controller) {
    controller->toggleOverlay();
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Controller* controller = createUIOverlay();
    int hotkey = XStringToKeysym((*(Config::root))["hotkey"].
                                 asString().c_str());
    int modifier = (*(Config::root))["hotkey_modifier"].asInt();
    HotKey::configureHotkey(hotkey, modifier, onHotkeyPress, controller);
    int result = app.exec();
    UIOverlay::terminate();
    HotKey::dispose();
    delete controller;
    return result;
}
