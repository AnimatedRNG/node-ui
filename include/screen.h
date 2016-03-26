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

#include <unordered_map>

#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QWidget>
#include <QPainter>
#include <QTime>

#include "util.h"
#include "nodesprite.h"

class Screen : public QWidget {

    Q_OBJECT
    
  public:
    Screen(QWidget* parent = 0);
    Screen(Screen&&) =
        default;                                                                            // Move constructor
    ~Screen();                                                                              // Destructor
    Screen& operator= (const Screen&)& =
        default;                                                                            // Copy assignment operator
    Screen& operator= (Screen&&)& =
        default;                                                                            // Move assignment operator
        
    void setController(std::function<void(QKeyEvent*)> controller);
    
    void start();
    static void terminate();
    
    void selectNode(const std::pair<int, int>& position);
    void deselectNode(const std::pair<int, int>& position);
    void highlightNode(const std::pair<int, int>& position);
    
    void setNodeIcons(const std::pair<int, int>& position,
                      const std::vector<std::shared_ptr<QIcon>>& icons);
    void resetAllNodeIcons();
    
    std::pair<int, int> getResolution();
    
    static constexpr const char* WINDOW_NAME = "NodeUI";
    static constexpr int FRAMERATE = 60;
    
    static constexpr int HORIZONTAL_NODE_NUM = 3;
    static constexpr int VERTICAL_NODE_NUM = 3;
    
    static constexpr double HORIZONTAL_PADDING = 0.2;
    static constexpr double VERTICAL_PADDING = 0.2;
    
    int timerID;
    
  protected:
    void timerEvent(QTimerEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void paintEvent(QPaintEvent* event);
    void closeEvent(QCloseEvent* event);
  private:
    void render(QPainter& painter);
    
    util::WindowProperties properties;
    std::function<void(QKeyEvent*)> controller;
    
    std::unordered_map<std::pair<int, int>, std::shared_ptr<NodeSprite>, pairhash>
    nodesprites;
};