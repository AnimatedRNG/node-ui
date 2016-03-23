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