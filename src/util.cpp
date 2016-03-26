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

void util::renderQTImage(QPainter& painter, QPixmap image, int x, int y,
                         int width, int height, int* frame,
                         int frame_num,
                         int frame_delay) {
    if (frame != NULL)
        *frame = (*frame + 1) % (frame_num * frame_delay);
        
    QRectF dst(x, y, image.width(), image.height());
    if (width > 0 && height > 0) {
        dst.setWidth(width);
        dst.setHeight(height);
    }
    
    int w = image.width() / frame_num;
    int f;
    if (frame == NULL)
        f = 0;
    else
        f = *frame;
        
    QRectF src(w * (f / frame_delay), 0, w, image.height());
    
    painter.drawPixmap(dst, image, src);
}

std::pair<int, int> util::toScreenCoords(const WindowProperties& props,
        std::pair<double, double> coords) {
    std::pair<int, int> resolution = {props.width, props.height};
    
    return std::pair<int, int> { resolution.first * coords.first, resolution.second * coords.second };
}