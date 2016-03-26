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

#include "util.h"
#include "nodesprite.h"

std::unique_ptr<QPixmap> NodeSprite::unselected = nullptr;
bool NodeSprite::initialized = false;
constexpr double NodeSprite::NODE_WIDTH;
constexpr double NodeSprite::NODE_HEIGHT;
constexpr int NodeSprite::NUM_FRAMES;

NodeSprite::NodeSprite(const std::pair<int, int>& position,
                       const util::WindowProperties& winprops) :
    _position(position),
    frame(rand() % NodeSprite::NUM_FRAMES),
    size(),
    icons(),
    tint {255, 255, 255} {
    if (!initialized) {
        try {
            NodeSprite::loadAssets();
        } catch (std::runtime_error& e) {
            throw;
        }
        initialized = true;
    }
    this->size = util::toScreenCoords(winprops,
                                      NodeSprite::getIdealSize(winprops));
    this->current = NodeSprite::unselected->copy();
}

void NodeSprite::loadAssets() {
    QImage temp;
    temp.load("assets/Node_Unselected.png");
    NodeSprite::unselected = std::unique_ptr<QPixmap>(new QPixmap);
    NodeSprite::unselected->convertFromImage(temp);
}

void NodeSprite::destroyAssets() {
    // I guess QT does all our work now?
}

void NodeSprite::select() {
    uint8_t selected[] = {0xFF, 0xDF, 0x00};
    memcpy(&(this->tint), &selected, 3 * sizeof(int));
}

void NodeSprite::unselect() {
    uint8_t unselected[] = {255, 255, 255};
    memcpy(&(this->tint), &unselected, 3 * sizeof(int));
}

void NodeSprite::highlight() {
    uint8_t highlighted[] = {0x1E, 0x90, 0xFF};
    memcpy(&(this->tint), &highlighted, 3 * sizeof(int));
}

void NodeSprite::setIcons(const std::vector<std::shared_ptr<QIcon>>& icons) {
    this->icons = icons;
}

void NodeSprite::render(const util::WindowProperties& winprops,
                        QPainter& painter) {
    util::renderQTImage(painter, current,
                        this->_position.first, this->_position.second,
                        size.first, size.second, &frame, 4, 10);
    this->drawOverlay(painter);
    this->drawIcons(painter);
}

std::pair<double, double> NodeSprite::getIdealSize(const util::WindowProperties&
        winprops) {
    std::pair<int, int> resolution = {winprops.width, winprops.height};
    
    if (resolution.first > resolution.second)
        return std::pair<double, double> {NODE_WIDTH, NODE_HEIGHT* ((double) resolution.first / resolution.second)};
    else
        return std::pair<double, double> {NODE_WIDTH* ((double) resolution.second / resolution.first), NODE_HEIGHT};
}

void NodeSprite::drawOverlay(QPainter& painter) {
    painter.setBrush(QColor(tint[0], tint[1], tint[2], 100));
    QRectF idealRect = QRectF(this->_position.first, this->_position.second,
                              size.first, size.second);
    painter.drawEllipse(idealRect);
}

void NodeSprite::drawIcons(QPainter& painter) {
    auto renderIcon =
    [&](std::shared_ptr<QIcon> icon, int x, int y, int width, int height) {
        util::renderQTImage(painter, icon->pixmap(QSize(width, height)), x, y, width,
                            height, NULL, 1, 1);
    };
    
    // Avoid expensive stitching operations if we can
    if (icons.size() == 1) {
        renderIcon(icons[0], this->_position.first, this->_position.second, size.first,
                   size.second);
        return;
    }
    
    if (icons.size() <= 1)
        return;
        
    // Resizes icons to fit in area
    int numIcons = icons.size();
    int order = ((int) ceil(log2((double) numIcons)));
    int dx, dy, offset_x, offset_y;
    
    int size_x, size_y;
    
    if (order % 2 == 0) {
        size_x = size.first / (order);
        size_y = size.second / (order);
        dx = size.first / order;
        dy = size.second / order;
        offset_x = 0;
        offset_y = 0;
    } else {
        size_x = size.first / (order + 1);
        size_y = size.second / (order + 1);
        dx = size.first / (order + 1);
        dy = size.second / order;
        offset_x = 0;
        offset_y = size_y / (order + 1);
    }
    
    int index = 0;
    for (int y = offset_y; y < size.first; y += dy) {
        for (int x = offset_x; x < size.second; x += dx) {
            if (index < numIcons) {
                renderIcon(icons[index], this->_position.first + x,
                           this->_position.second + y, size_x, size_y);
            }
            index += 1;
        }
    }
}