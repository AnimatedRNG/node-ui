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
std::shared_ptr<Json::Value> Config::appRoot;

std::string Config::readFile(const std::string& filename) {
    std::ifstream filestream(filename);
    return std::string((std::istreambuf_iterator<char>(filestream)),
                       std::istreambuf_iterator<char>());
}

std::string Config::writeFile(const std::string& filename,
                              const std::string& contents) {
    std::ofstream filestream(filename);
    filestream << contents;
}

void Config::readConfig() {
    root = std::shared_ptr<Json::Value>(new Json::Value);
    
    Json::Reader reader;
    if (!reader.parse(readFile(Config::CONFIG_FILE), *root))
        throw std::runtime_error("Config file is not valid JSON");
}

std::shared_ptr<std::vector<std::pair<util::Command, util::vec2i>>>
Config::readApplications() {
    auto output =
        std::shared_ptr<std::vector<std::pair<util::Command, util::vec2i>>>(
            new std::vector<std::pair<util::Command, util::vec2i>>);
    if (appRoot == nullptr)
        appRoot = std::shared_ptr<Json::Value>(new Json::Value);
        
    Json::Reader reader;
    std::string fileContents = readFile(Config::APP_FILE);
    if (!reader.parse(fileContents, *appRoot))
        throw std::runtime_error("Application file is not valid JSON");
        
    const Json::Value applicationList = (*appRoot)["applications"];
    for (int index = 0; index < applicationList.size(); index++) {
        const Json::Value entry = applicationList[index];
        std::string command = entry["command"].asString();
        std::string name = entry["name"].asString();
        QString iconName = QString::fromStdString(entry["icon"].asString());
        
        auto icon = std::make_shared<QIcon>(QIcon::fromTheme(iconName));
        
        util::vec2i pathValues;
        if (entry.isMember("path")) {
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
        }
        
        std::pair<util::Command, util::vec2i> pos = {util::Command({name, command, icon}), pathValues};
        output->push_back(pos);
    }
    return output;
}

QColor Config::getColor(std::string name) {
    const Json::Value color = (*root)["colors"][name];
    if (color.size() != 4)
        throw std::runtime_error("Color " + name + " is invalid!");
    uint8_t colorArray[4];
    for (int i = 0; i < color.size(); i++)
        colorArray[i] = color[i].asInt();
    return QColor(colorArray[0], colorArray[1], colorArray[2], colorArray[3]);
}

void Config::updateApplicationList(std::string applicationDirectory) {
    DEBUG("Updating config with new applications from directory " <<
          applicationDirectory);
    tinydir_dir appDir;
    tinydir_open_sorted(&appDir, applicationDirectory.c_str());
    
    auto alreadyExistingApps = * Config::readApplications();
    std::unordered_map<std::string, util::Command> associations;
    for (auto& app : alreadyExistingApps)
        associations[app.first.name] = app.first;
        
    int newApps = 0;
    for (int i = 0; i < appDir.n_files; i++) {
        tinydir_file file;
        tinydir_readfile_n(&appDir, &file, i);
        
        if (file.is_dir)
            continue;
            
        std::istringstream contents(readFile(
                                        applicationDirectory + "/" +
                                        file.name));
        std::string line;
        
        const std::string nameStr = std::string("Name=");
        std::string name;
        const std::string execStr = std::string("Exec=");
        std::string exec;
        const std::string iconStr = std::string("Icon=");
        std::string icon;
        const std::string typeStr = std::string("Type=");
        std::string type;
        
        auto searchForString = [](const std::string & l, const std::string & search,
        std::string & output) {
            if (output.length() == 0 &&
                    strncmp(l.c_str(), search.c_str(), search.length()) == 0 &&
                    l.length() != search.length()) {
                output = l.substr(search.length(), l.length());
            }
        };
        
        while (std::getline(contents, line)) {
            searchForString(line, nameStr, name);
            searchForString(line, execStr, exec);
            searchForString(line, iconStr, icon);
            searchForString(line, typeStr, type);
        }
        if (name.length() == 0 || icon.length() == 0 || exec.length() == 0 ||
                type != "Application")
            continue;
            
        std::regex escape_wildcard("(%\\S*)");
        if (associations.count(name) > 0)
            continue;
            
        exec = std::regex_replace(exec, escape_wildcard, "");
        
        int numApps = (*appRoot)["applications"].size();
        (*appRoot)["applications"][numApps]["name"] = name;
        (*appRoot)["applications"][numApps]["command"] = exec;
        (*appRoot)["applications"][numApps]["icon"] = icon;
        newApps++;
    }
    
    tinydir_close(&appDir);
    
    DEBUG("Found " << newApps << " new applications.");
    
    Json::StyledWriter writer;
    Config::writeFile(Config::APP_FILE, writer.write(*appRoot));
}
