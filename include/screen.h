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
#include <set>

#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QEvent>
#include <QWidget>
#include <QPainter>
#include <QTime>

#include "util.h"
#include "config.h"
#include "nodesprite.h"

class UIOverlay : public QWidget {

    Q_OBJECT
    
  public:
    typedef std::pair<std::pair<int, int>,
            std::pair<int, int>> coord_pair;
            
    UIOverlay(QWidget* parent = 0);
    UIOverlay(UIOverlay&&) =
        default;                                                                            // Move constructor
    ~UIOverlay();                                                                              // Destructor
    UIOverlay& operator= (const UIOverlay&)& =
        default;                                                                            // Copy assignment operator
    UIOverlay& operator= (UIOverlay&&)& =
        default;                                                                            // Move assignment operator
        
    void setController(std::function<void(QKeyEvent*)> controller);
    void setFocusHandler(std::function<void(const bool& hasFocus)> handler);
    
    void start();
    static void terminate();
    
    void selectNode(const std::pair<int, int>& position);
    void deselectNode(const std::pair<int, int>& position);
    void highlightNode(const std::pair<int, int>& position);
    void drawPath(const std::pair<int, int>& startPosition,
                  const std::pair<int, int>& endPosition);
                  
    void setNodeIcons(const std::pair<int, int>& position,
                      const std::vector<std::shared_ptr<QIcon>>& icons);
    void deselectAllNodes();
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
    
    void changeEvent(QEvent* event);
    
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
  private:
    void render(QPainter& painter);
    
    util::WindowProperties properties;
    std::function<void(QKeyEvent*)> controller;
    std::function<void(const bool& hasFocus)> focusHandler;
    
    std::unordered_map<std::pair<int, int>, std::shared_ptr<NodeSprite>, pairhash>
    nodesprites;
    
    std::set<coord_pair> pathOverlay;
};
