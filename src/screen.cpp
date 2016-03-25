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

#include <exception>
#include <typeinfo>
#include <stdexcept>
#include <time.h>

#include "util.h"
#include "screen.h"


bool Screen::initialized = false;

Screen::Screen() :
    properties {NULL, NULL},
    width(0),
    height(0),
    nodesprites() {
    if (!initialized) {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
        Screen::initialized = true;
    }
    
    srand(time(NULL));
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    std::pair<double, double> padding =
        std::make_pair(current.w * HORIZONTAL_PADDING,
                       current.h * VERTICAL_PADDING);
    this->width = current.w - padding.first;
    this->height = current.h - padding.second;
    this->properties.window = SDL_CreateShapedWindow(Screen::WINDOW_NAME,
                              this->width / 2 - padding.first / 2,
                              this->height / 2 - padding.second / 2,
                              this->width, this->height,
                              SDL_WINDOW_FULLSCREEN_DESKTOP |
                              SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALLOW_HIGHDPI);
    // Recenter window just in case
    SDL_SetWindowPosition(this->properties.window, SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED);
                          
    this->properties.renderer = SDL_CreateRenderer(this->properties.window, -1,
                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
                                
    if (this->properties.renderer == nullptr)
        throw std::runtime_error("Failed to create renderer: " + std::string(
                                     SDL_GetError()));
                                     
    std::pair<double, double> node_size = NodeSprite::getIdealSize(
            this->properties);
    for (double i = 0.50; i < HORIZONTAL_NODE_NUM; i++) {
        for (double j = 0.50; j < VERTICAL_NODE_NUM; j++) {
            std::pair<double, double> spacing = {1.0 / HORIZONTAL_NODE_NUM, 1.0 / VERTICAL_NODE_NUM};
            std::pair<int, int> position = util::toScreenCoords(this->properties, {
                spacing.first * i - node_size.first / 2.0,
                spacing.second * j - node_size.second / 2.0
            });
            
            std::pair<int, int> index = {(int) i, (int) j};
            std::shared_ptr<NodeSprite> sprite = std::shared_ptr<NodeSprite>(new NodeSprite(
                    position, this->properties));
            this->nodesprites.insert(std::make_pair(index, sprite));
        }
    }
}

Screen::~Screen() {
    SDL_DestroyRenderer(this->properties.renderer);
    SDL_DestroyWindow(this->properties.window);
}

void Screen::setController(std::function<void(SDL_Event)> controller) {
    this->controller = controller;
}

void Screen::start() {
    SDL_Event event;
    
    while (true) {
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    return;
                case SDL_WINDOWEVENT_CLOSE:
                    return;
                default:
                    if (this->controller != nullptr)
                        this->controller(event);
            }
        }
        
        try {
            this->render();
        } catch (...) {
            std::exception_ptr p = std::current_exception();
            std::clog << (p ? p.__cxa_exception_type() -> name() : "null") << std::endl;
            
            Screen::terminate();
        }
        
        SDL_Delay(1000 / Screen::FRAMERATE);
    }
}

void Screen::terminate() {
    std::cout << "Destroying assets" << std::endl;
    NodeSprite::destroyAssets();
    
    SDL_Quit();
}

void Screen::selectNode(const std::pair<int, int>& position) {
    this->nodesprites.at(position)->select();
}

void Screen::deselectNode(const std::pair<int, int>& position) {
    this->nodesprites.at(position)->unselect();
}

void Screen::highlightNode(const std::pair<int, int>& position) {
    this->nodesprites.at(position)->highlight();
}

std::pair<int, int> Screen::getResolution() {
    return std::make_pair(this->width, this->height);
}

void Screen::render() {
    SDL_RenderClear(this->properties.renderer);
    
    for (auto& map : this->nodesprites)
        map.second->render(this->properties);
        
    SDL_RenderPresent(this->properties.renderer);
}