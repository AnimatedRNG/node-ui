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

#include "controller.h"


void onReceive(std::string str, Controller* controller) {
    DEBUG(str);
    controller->model = controller->model->select(str);
    
    controller->loadIcons();
    
    controller->updateView();
}

void Controller::updateView() {
    auto path = this->model->getPath();
    for (auto it = path->begin(); it != path->end(); it++)
        this->screen->selectNode(*it);
    this->screen->highlightNode(*(path->end() - 1));
}

void Controller::loadIcons() {
    this->screen->resetAllNodeIcons();
    std::vector<std::string> directions =
        * (this->model->getViableDirections());
    for (auto direction : directions) {
        auto possibilities = * (this->model->getCommandsInDirection(direction));
        std::pair<int, int> currentPosition = this->model->getCurrentPosition()
                                              + getDelta(direction);
        std::vector<std::shared_ptr<QIcon>> icons;
        for (auto& possibility : possibilities)
            icons.push_back(possibility.icon);
        this->screen->setNodeIcons(currentPosition, icons);
    }
}