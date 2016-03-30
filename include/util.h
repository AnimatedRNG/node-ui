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

#include <iostream>

#include <stdio.h>
#include <string>
#include <memory>
#include <vector>
#include <exception>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <stdlib.h>
#include <string.h>

#include <QApplication>
#include <QProcess>
#include <QKeyEvent>
#include <QWidget>
#include <QPainter>
#include <QTime>

#define DEBUG(a)            std::cout << a << std::endl
#define DEBUGARR(a)         for (int i = 0; i < std::end(a) - std::begin(a); i++) std::cout << a[i] << " "; std::cout << std::endl;
#define ERROR(a)            std::cerr << a << std::endl

struct pairhash {
  public:
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U>& x) const {
        return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
    }
};

namespace util {

    typedef std::vector<std::pair<int, int>> vec2i;
    
    static constexpr int GRID_WIDTH = 3;
    static constexpr int GRID_HEIGHT = 3;
    
    struct WindowProperties {
        int width;
        int height;
    };
    
    struct Command {
        std::string name;
        std::string command;
        std::shared_ptr<QIcon> icon;
    };
    
    static std::unordered_map<int, std::string> keyToString = {
        {Qt::Key_Up, "Up"},
        {Qt::Key_Down, "Down"},
        {Qt::Key_Left, "Left"},
        {Qt::Key_Right, "Right"},
        {Qt::Key_Backspace, "Backspace"},
        {Qt::Key_Escape, "Escape"},
    };
    
    inline std::ostream& operator<<(std::ostream& strm, Command const& command) {
        return strm << command.name;
    }
    
    void renderQTImage(QPainter& painter, QPixmap image, int x, int y,
                       int width, int height, int* frame,
                       int frame_num,
                       int frame_delay);
                       
    std::pair<int, int> toScreenCoords(const WindowProperties& props,
                                       std::pair<double, double> coords);
                                       
    void executeCommand(std::string command);
}

// Defined int pair addition
inline std::pair<int, int> operator+(const std::pair<int, int>& l,
                                     const std::pair<int, int>& r) {
    return {l.first + r.first, l.second + r.second};
}

// Defined int pair subtraction
inline std::pair<int, int> operator-(const std::pair<int, int>& l,
                                     const std::pair<int, int>& r) {
    return {l.first - r.first, l.second - r.second};
}

inline std::ostream& operator<<(std::ostream& strm,
                                std::pair<int, int> const& node) {
    strm << "(" << node.first << ", " << node.second << ")";
    return strm;
}