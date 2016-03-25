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
    controller->updateView();
}

void Controller::updateView() {
    auto path = * (this->model->getPath());
    for (auto& node : path)
        this->screen->selectNode(node);
    this->screen->highlightNode(*(path.end() - 1));
}