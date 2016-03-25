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

Controller* createScreen() {
    std::vector<Model::command_position> paths;
    paths.push_back(std::make_pair(util::Command {"Example"}, util::vec2i{ {1, 1}, {1, 2}, {2, 2} }));
    paths.push_back(std::make_pair(util::Command {"Example2"}, util::vec2i{ {1, 1}, {1, 0}, {0, 0} }));
    paths.push_back(std::make_pair(util::Command {"Example3"}, util::vec2i{ {1, 1}, {1, 0}, {2, 0} }));
    
    std::shared_ptr<Screen> screen(new Screen);
    std::shared_ptr<Model> model(new Model(
                                     std::make_shared<std::vector<Model::command_position>>(paths)));
    Controller* controller = new Controller(model, screen);
    screen->show();
    screen->start();
    return controller;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Config::readConfig();
    Controller* controller = createScreen();
    int result = app.exec();
    Screen::terminate();
    return result;
}