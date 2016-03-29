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

#pragma once

#include <string>
#include <fstream>
#include <streambuf>

#include <QIcon>
#include <QColor>
#include <QString>

#include <json/config.h>
#include <json/value.h>
#include <json/reader.h>

#include "util.h"

class Config {

  public:
    static std::shared_ptr<Json::Value> root;
    
    static std::string readFile(const std::string& filename);
    
    static void readConfig();
    
    static std::shared_ptr<std::vector<std::pair<util::Command, util::vec2i>>>
    readApplications();
    
    static QColor getColor(std::string name);
};
