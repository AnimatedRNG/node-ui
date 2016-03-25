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

void NodeSprite::render(const util::WindowProperties& winprops,
                        QPainter& painter) {
    util::renderQTImage(painter, current,
                        this->_position.first, this->_position.second,
                        size.first, size.second, &frame, 4, 10);
    this->drawOverlay(painter);
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