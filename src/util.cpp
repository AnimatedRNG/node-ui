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

GLuint util::LoadShaders(const char* vertex_file_path,
                         const char* fragment_file_path) {
                         
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    
    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if (VertexShaderStream.is_open()) {
        std::string Line = "";
        while (getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    } else {
        throw std::runtime_error("Failed to open vertex shader.");
    }
    
    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::string Line = "";
        while (getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }
    
    GLint Result = GL_FALSE;
    int InfoLogLength;
    
    
    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const* VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);
    
    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL,
                           &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }
    
    
    
    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const* FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
    
    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL,
                           &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }
    
    
    
    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
    
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }
    
    
    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);
    
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    
    return ProgramID;
}

void util::renderTexture(SDL_Texture* tex, SDL_Renderer* ren, int x, int y,
                         int width, int height,
                         int* frame, int frame_num, int frame_delay) {
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    
    if (width > 0 && height > 0) {
        dst.w = width;
        dst.h = height;
    } else
        SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
        
    SDL_Rect* src = new SDL_Rect();
    if (frame == NULL)
        src = NULL;
    else {
        SDL_Rect tex_src;
        SDL_QueryTexture(tex, NULL, NULL, &tex_src.w, &tex_src.h);
        src->w = tex_src.w / frame_num;
        src->h = tex_src.h;
        src->x = src->w * (*frame / frame_delay);
        src->y = 0;
        
        *frame = (*frame + 1) % (frame_num * frame_delay);
    }
    
    SDL_RenderCopy(ren, tex, src, &dst);
    delete src;
}

SDL_Texture* util::loadPNG(const std::string& assetName,
                           SDL_Renderer* renderer) {
    unsigned width, height;
    std::vector<unsigned char> image_buffer;
    unsigned error = lodepng::decode(image_buffer, width, height, assetName);
    int image_buf_size = image_buffer.size();
    
    if (error != 0)
        throw std::runtime_error("Error loading assets: " + error);
        
    Uint32 rmask, gmask, bmask, amask;
    
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
    
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask,
                           bmask, amask);
                           
    for (int i = 0; i < image_buf_size; i++) {
        uint32_t shift = ((i % 4) * 8);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        ((uint32_t*) surface->pixels) [i / 4] |= ((uint32_t) image_buffer[i]) >> shift;
#else
        ((uint32_t*) surface->pixels) [i / 4] |= ((uint32_t) image_buffer[i]) << shift;
#endif
    }
    
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (tex == nullptr)
        throw std::runtime_error("Failed to load asset " + assetName + ": " +
                                 std::string(SDL_GetError()));
                                 
    return tex;
}

std::pair<int, int> util::toScreenCoords(const WindowProperties& props,
        std::pair<double, double> coords) {
    std::pair<int, int> resolution;
    SDL_GetWindowSize(props.window, &resolution.first, &resolution.second);
    
    return std::pair<int, int> { resolution.first * coords.first, resolution.second * coords.second };
}