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

#include "config.h"

std::shared_ptr<Json::Value> Config::root;

std::string Config::readFile(const std::string& filename) {
    std::ifstream filestream(filename);
    return std::string((std::istreambuf_iterator<char>(filestream)),
                       std::istreambuf_iterator<char>());
}

void Config::readConfig() {
    root = std::shared_ptr<Json::Value>(new Json::Value);
    
    Json::Reader reader;
    if (!reader.parse(readFile("assets/config/config.json"), *root))
        throw std::runtime_error("Config file is not valid JSON");
}

std::shared_ptr<std::vector<std::pair<util::Command, util::vec2i>>>
Config::readApplications() {
    auto output =
        std::shared_ptr<std::vector<std::pair<util::Command, util::vec2i>>>(
            new std::vector<std::pair<util::Command, util::vec2i>>);
            
    Json::Value appRoot;
    
    Json::Reader reader;
    std::string fileContents = readFile("assets/config/applications.json");
    if (!reader.parse(fileContents, appRoot))
        throw std::runtime_error("Application file is not valid JSON");
        
    const Json::Value applicationList = appRoot["applications"];
    for (int index = 0; index < applicationList.size(); index++) {
        const Json::Value entry = applicationList[index];
        std::string name = entry["command"].asString();
        QString iconName = QString::fromStdString(entry["icon"].asString());
        
        auto icon = std::make_shared<QIcon>(QIcon::fromTheme(iconName));
        
        util::vec2i pathValues;
        const Json::Value commandPath = entry["path"];
        for (int i = 0; i < commandPath.size(); i++) {
            const Json::Value coordinate = commandPath[i];
            std::string errorPrefix = "Path associated with command "
                                      + entry["command"].asString();
            if (coordinate.size() != 2)
                throw std::runtime_error(errorPrefix
                                         + " does " + "not have the" +
                                         " correct number" + " of coordinates");
            if (!coordinate[0].isInt() || !coordinate[1].isInt())
                throw std::runtime_error(errorPrefix + " does not have valid "
                                         + "coordinates");
            std::pair<int, int> coord = {coordinate[0].asInt(),
                                         coordinate[1].asInt()
                                        };
            pathValues.push_back(coord);
        }
        
        std::pair<util::Command, util::vec2i> pos = {util::Command({name, icon}), pathValues};
        output->push_back(pos);
    }
    return output;
}