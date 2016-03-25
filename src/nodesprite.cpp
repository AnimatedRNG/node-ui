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

bool NodeSprite::initialized = false;
SDL_Texture* NodeSprite::unselected = NULL;
constexpr double NodeSprite::NODE_WIDTH;
constexpr double NodeSprite::NODE_HEIGHT;
constexpr int NodeSprite::NUM_FRAMES;

NodeSprite::NodeSprite(const std::pair<int, int>& position,
                       const util::WindowProperties& winprops) :
    _position(position),
    frame(rand() % NodeSprite::NUM_FRAMES),
    tint {255, 255, 255} {
    if (!initialized) {
        try {
            NodeSprite::loadAssets(winprops.renderer);
        } catch (std::runtime_error& e) {
            throw;
        }
        initialized = true;
    }
}

void NodeSprite::loadAssets(SDL_Renderer* renderer) {
    NodeSprite::unselected = util::loadPNG("assets/Node_Unselected.png", renderer);
}

void NodeSprite::destroyAssets() {
    SDL_DestroyTexture(NodeSprite::unselected);
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
    uint8_t unselected[] = {0x1E, 0x90, 0xFF};
    memcpy(&(this->tint), &unselected, 3 * sizeof(int));
}

void NodeSprite::render(const util::WindowProperties& winprops) {
    std::pair<int, int> size = util::toScreenCoords(winprops,
                               NodeSprite::getIdealSize(winprops));
                               
    SDL_SetTextureColorMod(NodeSprite::unselected, tint[0], tint[1], tint[2]);
    util::renderTexture(NodeSprite::unselected, winprops.renderer,
                        this->_position.first, this->_position.second,
                        size.first, size.second, &frame, 4, 10);
    SDL_SetTextureColorMod(NodeSprite::unselected, 255, 255, 255);
}

std::pair<double, double> NodeSprite::getIdealSize(const util::WindowProperties&
        winprops) {
    std::pair<int, int> resolution;
    SDL_GetWindowSize(winprops.window, &resolution.first, &resolution.second);
    
    if (resolution.first > resolution.second)
        return std::pair<double, double> {NODE_WIDTH, NODE_HEIGHT* ((double) resolution.first / resolution.second)};
    else
        return std::pair<double, double> {NODE_WIDTH* ((double) resolution.second / resolution.first), NODE_HEIGHT};
}