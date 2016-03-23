#pragma once

#include <algorithm>

#include "SDL.h"

#include "util.h"

class NodeSprite {
  public:
    NodeSprite(const std::pair<int, int>& position,
               const util::WindowProperties& winprops);
    NodeSprite(NodeSprite&&) = default;

    static void loadAssets(SDL_Renderer* renderer);
    static void destroyAssets();
    static std::pair<double, double> getIdealSize(const util::WindowProperties&
            winprops);

    void render(const util::WindowProperties& winprops);

    static constexpr double NODE_WIDTH = 0.1;
    static constexpr double NODE_HEIGHT = 0.1;

    static constexpr int NUM_FRAMES = 4;

    std::pair<int, int> _position;
  private:
    static SDL_Texture* unselected;

    static bool initialized;

    int frame;
    uint8_t tint[3];
};