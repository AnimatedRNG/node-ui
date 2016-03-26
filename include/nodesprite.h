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

#include <algorithm>
#include <vector>

#include <QImage>
#include <QColor>
#include <QIcon>
#include <QPainter>

#include "util.h"

class NodeSprite {
  public:
    NodeSprite(const std::pair<int, int>& position,
               const util::WindowProperties& winprops);
    NodeSprite(const NodeSprite& nodesprite) = default;
    
    static void loadAssets();
    static void destroyAssets();
    static std::pair<double, double> getIdealSize(const util::WindowProperties&
            winprops);
            
    void select();
    void unselect();
    void highlight();
    
    void setIcons(const std::vector<std::shared_ptr<QIcon>>& icons);
    
    void render(const util::WindowProperties& winprops, QPainter& painter);
    
    static constexpr double NODE_WIDTH = 0.1;
    static constexpr double NODE_HEIGHT = 0.1;
    
    static constexpr int NUM_FRAMES = 4;
    
    std::pair<int, int> _position;
  private:
    void drawOverlay(QPainter& painter);
    
    static std::unique_ptr<QPixmap> unselected;
    QPixmap current;
    std::pair<int, int> size;
    
    static bool initialized;
    
    std::vector<std::shared_ptr<QIcon>> icons;
    
    int frame;
    uint8_t tint[3];
};