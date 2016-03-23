#pragma once

#include "SDL.h"
#include "SDL_shape.h"

#include "util.h"
#include "nodesprite.h"

class Screen {
  public:
    Screen(std::function<void(SDL_Event)> controller);
    Screen(Screen&&) =
        default;                                                                            // Move constructor
    ~Screen();                                                                              // Destructor
    Screen& operator= (const Screen&)& =
        default;                                                                            // Copy assignment operator
    Screen& operator= (Screen&&)& =
        default;                                                                            // Move assignment operator

    void start();
    static void terminate();

    std::pair<int, int> getResolution();

    static constexpr const char* WINDOW_NAME = "NodeUI";
    static constexpr int FRAMERATE = 60;

    static constexpr int HORIZONTAL_NODE_NUM = 3;
    static constexpr int VERTICAL_NODE_NUM = 3;

    static bool initialized;
  private:
    void render();

    util::WindowProperties properties;
    std::function<void(SDL_Event)> controller;
    int width, height;

    std::vector<NodeSprite> nodesprites;
};