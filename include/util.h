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

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "SDL.h"
#include "lodepng/lodepng.h"

#define DEBUG(a)            std::cout << a << std::endl
#define DEBUGARR(a)         for (int i = 0; i < std::end(a) - std::begin(a); i++) std::cout << a[i] << " "; std::cout << std::endl;
#define ERROR(a)            std::cerr << a << std::endl

namespace util {

    typedef std::vector<std::pair<int, int>> vec2i;
    
    static constexpr int GRID_WIDTH = 3;
    static constexpr int GRID_HEIGHT = 3;
    
    struct WindowProperties {
        SDL_Window* window;
        SDL_Renderer* renderer;
    };
    
    struct Command {
        std::string name;
    };
    
    inline std::ostream& operator<<(std::ostream& strm, Command const& command) {
        return strm << command.name;
    }
    
    GLuint LoadShaders(const char* vertex_file_path,
                       const char* fragment_file_path);
                       
    SDL_Texture* loadPNG(const std::string& assetName, SDL_Renderer* renderer);
    void renderTexture(SDL_Texture* texture, SDL_Renderer* renderer, int x, int y,
                       int width = -1, int height = -1, int* frame = NULL, int frame_num = -1,
                       int frame_delay = 1);
                       
    std::pair<int, int> toScreenCoords(const WindowProperties& props,
                                       std::pair<double, double> coords);
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