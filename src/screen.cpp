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

Screen::Screen(std::function<void(SDL_Event)> controller) :
    properties {NULL, NULL},
    controller(controller),
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
    this->width = current.w;
    this->height = current.h;
    this->properties.window = SDL_CreateWindow(Screen::WINDOW_NAME, 0, 0,
                              this->width, this->height,
                              SDL_WINDOW_MAXIMIZED | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_MAXIMIZED |
                              SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALLOW_HIGHDPI);
                              
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
            std::pair<int, int> position = util::toScreenCoords(this->properties, {spacing.first * i - node_size.first / 2.0,
                                           spacing.second * j - node_size.second / 2.0
                                                                                  });
            this->nodesprites.push_back(NodeSprite(position, this->properties));
        }
    }
}

Screen::~Screen() {
    SDL_DestroyRenderer(this->properties.renderer);
    SDL_DestroyWindow(this->properties.window);
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

std::pair<int, int> Screen::getResolution() {
    return std::make_pair(this->width, this->height);
}

void Screen::render() {
    SDL_RenderClear(this->properties.renderer);
    
    for (auto& node : this->nodesprites)
        node.render(this->properties);
        
    SDL_RenderPresent(this->properties.renderer);
}