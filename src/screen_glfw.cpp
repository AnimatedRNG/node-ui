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

#include "screen_glfw.h"
#include "util.h"

bool Screen::initialized = false;
bool NodeSprite::initialized = false;
std::vector<unsigned char> NodeSprite::unselected;
GLfloat NodeSprite::g_quad_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
};
GLuint* NodeSprite::unselected_id = new GLuint(1);
GLuint NodeSprite::unselected_program = 0;

NodeSprite::NodeSprite(std::pair<int, int> resolution,
                       std::pair<double, double> position) :
    idealSize(),
    quad(),
    quadVerticesHandle(),
    _position(position),
    mvp() {
    if (!NodeSprite::initialized) {
        DEBUG("Loading Node sprites");
        NodeSprite::loadAssets(resolution);
    }
    double width = NODE_WIDTH;
    double height = NODE_HEIGHT;
    this->idealSize = {((double) resolution.second / resolution.first)* width, height};
    
    GLfloat quadVerticesData[15];
    std::copy(std::begin(g_quad_vertex_buffer_data),
              std::end(g_quad_vertex_buffer_data), std::begin(quadVerticesData));
    for (int i = 0; i < 15; i += 3) {
        quadVerticesData[i] = quadVerticesData[i] * this->idealSize.first +
                              _position.first;
        quadVerticesData[i + 1] = quadVerticesData[i + 1] * this->idealSize.second +
                                  _position.second;
    }
    
    glGenBuffers(1, &quad);
    glBindBuffer(GL_ARRAY_BUFFER, this->quad);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerticesData), quadVerticesData,
                 GL_STATIC_DRAW);
    this->quadVerticesHandle = glGetAttribLocation(NodeSprite::unselected_program,
                               "vertexPosition_modelspace");
                               
    mvp = glGetUniformLocation(NodeSprite::unselected_program, "MVP");
    
    DEBUGARR(quadVerticesData);
}

NodeSprite::~NodeSprite() {
    glDeleteBuffers(1, &quad);
    glDeleteProgram(unselected_program);
}

void NodeSprite::loadAssets(std::pair<int, int> resolution) {
    unsigned width, height;
    unsigned error = lodepng::decode(NodeSprite::unselected, width, height,
                                     "assets/Node_Unselected.png");
                                     
    if (error != 0)
        throw std::runtime_error("Error loading assets: " + error);
        
    glGenTextures(1, NodeSprite::unselected_id);
    glBindTexture(GL_TEXTURE_2D, *(NodeSprite::unselected_id));
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, &(NodeSprite::unselected)[0]);
                 
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glGenerateMipmap(GL_TEXTURE_2D);
    
    NodeSprite::unselected_program = util::LoadShaders("assets/node_unselected.vs",
                                     "assets/node_unselected.fs");
                                     
    NodeSprite::initialized = true;
}

void NodeSprite::render(glm::mat4 view, glm::mat4 projection) {
    //glLoadIdentity();
    // I'm going to hell for this
    /*glBegin(GL_QUADS);
      glTexCoord2d( 0,  0); glVertex2f(    0,      0);
      glTexCoord2d( 1.0,  0); glVertex2f(this->idealSize.first,      0);
      glTexCoord2d( 1.0, 1.0); glVertex2f(this->idealSize.first, this->idealSize.second);
      glTexCoord2d( 0, 1.0); glVertex2f(0, this->idealSize.second);
    glEnd();*/
    glUseProgram(NodeSprite::unselected_program);
    
    glEnableVertexAttribArray(this->quadVerticesHandle);
    glBindBuffer(GL_ARRAY_BUFFER, this->quad);
    glVertexAttribPointer(this->quadVerticesHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(this->quadVerticesHandle);
}

Screen::Screen(std::function<void(int, const char*)> error_callback,
               std::function<void(GLFWwindow*, int, int, int, int)> key_callback) :
    _error_callback(error_callback),
    _key_callback(key_callback),
    nodeSprites() {
    typedef void error_function_t(int, const char*);
    typedef void key_callback_t(GLFWwindow*, int, int, int, int);
    error_function_t* error_callback_ptr =
        this->_error_callback.target<error_function_t>();
    glfwSetErrorCallback(error_callback_ptr);
    
    if (!Screen::initialized)
        if (!glfwInit())
            throw std::runtime_error("Unable to initialize GLFW");
        else
            this->initialized = true;
            
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    glfwWindowHint(GLFW_DECORATED, false);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    
    this->window = glfwCreateWindow(mode->width, mode->height, Screen::WINDOW_NAME,
                                    primary, NULL);
                                    
    if (!window) {
        throw std::runtime_error("Unable to make new window");
    }
    
    glfwMakeContextCurrent(window);
    
    if (glewInit() != GLEW_OK)
        throw std::runtime_error("Unable to initialize GLEW");
        
    key_callback_t* key_callback_ptr = this->_key_callback.target<key_callback_t>();
    
    glfwSetKeyCallback(window, key_callback_ptr);
    
    /*glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);*/
    
    glClearColor(0, 0, 0, 0);
    
    NodeSprite::loadAssets(this->getResolution());
    for (double i = 0; i < 1; i++)
        this->nodeSprites.push_back(NodeSprite(this->getResolution(),
                                               std::make_pair(0.5, 0.5)));
}

void Screen::start() {
    while (!glfwWindowShouldClose(window)) {
        this->render();
        
        glfwSwapBuffers(this->window);
        glfwPollEvents();
    }
}

void Screen::render() {
    float ratio;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;
    
    // I'm going to hell for this
    /*glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);*/
    
    glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0),
                                 glm::vec3(0, 1, 0));
                                 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (auto node = nodeSprites.begin(); node != nodeSprites.end(); node++)
        node->render(projection, view);
}

Screen::~Screen() {
    glfwDestroyWindow(this->window);
}

void Screen::terminate() {
    glfwTerminate();
}

std::pair<int, int> Screen::getResolution() {
    int width, height;
    glfwGetFramebufferSize(this->window, &width, &height);
    return std::make_pair(width, height);
}