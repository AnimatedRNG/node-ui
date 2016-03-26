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
    auto apps = Config::readApplications();
    
    // TODO: Fix odd memory corruption that happens around here on rare occasions
    std::shared_ptr<UIOverlay> screen(new UIOverlay);
    std::shared_ptr<Model> model(new Model(apps));
    Controller* controller = new Controller(model, screen);
    screen->show();
    screen->start();
    return controller;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Controller* controller = createUIOverlay();
    int result = app.exec();
    UIOverlay::terminate();
    return result;
}