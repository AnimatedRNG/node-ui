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

#include "util.h"

#include <lodepng.h>

class NodeSprite {
  public:
    NodeSprite(std::pair<int, int> resolution, std::pair<double, double> position);
    NodeSprite(NodeSprite&&) = default;
    ~NodeSprite();
    
    static void loadAssets(std::pair<int, int> resolution);
    void render(glm::mat4 view, glm::mat4 projection);
    
    static constexpr double NODE_WIDTH = 0.1;
    static constexpr double NODE_HEIGHT = 0.1;
  private:
    static std::vector<unsigned char> unselected;
    static GLuint* unselected_id;
    
    static GLuint unselected_program;
    
    GLuint quad;
    GLuint quadVerticesHandle;
    GLuint mvp;
    
    std::pair<double, double> _position;
    
    static bool initialized;
    static GLfloat g_quad_vertex_buffer_data[];
    
    std::pair<double, double> idealSize;
};

class Screen {
  public:
    Screen(std::function<void(int, const char*)> error_callback,
           std::function<void(GLFWwindow*, int, int, int, int)>
           key_callback);                   // Constructor
    Screen(Screen&&) =
        default;                                                             // Move constructor
    ~Screen();                                                                              // Destructor
    Screen& operator= (const Screen&)& =
        default;                                            // Copy assignment operator
    Screen& operator= (Screen&&)& =
        default;                                                // Move assignment operator
        
    void start();
    static void terminate();
    
    std::pair<int, int> getResolution();
    
    static constexpr const char* WINDOW_NAME = "NodeUI";
    
    static bool initialized;
  private:
    void render();
    
    std::function<void(int, const char*)> _error_callback;
    std::function<void(GLFWwindow*, int, int, int, int)> _key_callback;
    
    GLFWwindow* window;
    
    std::vector<NodeSprite> nodeSprites;
};