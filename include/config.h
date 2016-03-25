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

#include <istream>
#include <fstream>

#include <json/config.h>
#include <json/value.h>
#include <json/reader.h>

#include "util.h"

namespace Config {

    static std::shared_ptr<Json::Value> root;
    
    void readConfig() {
        std::ifstream configFile;
        configFile.open("assets/config.json");
        
        root = std::shared_ptr<Json::Value>(new Json::Value);
        
        Json::Reader reader;
        if (!reader.parse((std::istream&) configFile, *root))
            std::runtime_error("Config file is not valid JSON");
    }
};