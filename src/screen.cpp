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

#include <exception>
#include <typeinfo>
#include <stdexcept>
#include <time.h>

#include "util.h"
#include "screen.h"

UIOverlay::UIOverlay(QWidget* parent) :
    QWidget(parent),
    properties {0, 0},
    nodesprites() {
    
    srand(time(NULL));
    
    QRect rec = QApplication::desktop()->screenGeometry();
    std::pair<double, double> padding =
        std::make_pair(rec.width() * HORIZONTAL_PADDING,
                       rec.height() * VERTICAL_PADDING);
    this->properties.width = rec.width() - padding.first;
    this->properties.height = rec.height() - padding.second;
    this->resize(this->properties.width, this->properties.height);
    this->setFixedSize(this->properties.width, this->properties.height);
    this->move(QApplication::desktop()->availableGeometry().center() -
               this->rect().center());
               
    this->setStyleSheet("background:transparent;");
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint);
    
    std::pair<double, double> node_size = NodeSprite::getIdealSize(
            this->properties);
    for (double i = 0.50; i < HORIZONTAL_NODE_NUM; i++) {
        for (double j = 0.50; j < VERTICAL_NODE_NUM; j++) {
            std::pair<double, double> spacing = {1.0 / HORIZONTAL_NODE_NUM, 1.0 / VERTICAL_NODE_NUM};
            std::pair<int, int> position = util::toScreenCoords(this->properties, {
                spacing.first * i - node_size.first / 2.0,
                spacing.second * j - node_size.second / 2.0
            });
            
            std::pair<int, int> index = {(int) i, (int) j};
            std::shared_ptr<NodeSprite> sprite = std::shared_ptr<NodeSprite>(new NodeSprite(
                    position, this->properties));
            this->nodesprites.insert(std::make_pair(index, sprite));
        }
    }
}

UIOverlay::~UIOverlay() {
    // I guess QT does all our work now?
}

void UIOverlay::timerEvent(QTimerEvent* event) {
    Q_UNUSED(event);
    repaint();
}

void UIOverlay::keyPressEvent(QKeyEvent* event) {
    if (this->controller != nullptr)
        this->controller(event);
}

void UIOverlay::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter qp(this);
    
    try {
        this->render(qp);
    } catch (...) {
        std::exception_ptr p = std::current_exception();
        std::clog << (p ? p.__cxa_exception_type() -> name() : "null") << std::endl;
        
        UIOverlay::terminate();
    }
}

void UIOverlay::closeEvent(QCloseEvent* event) {
    QApplication::quit();
}

void UIOverlay::setController(std::function<void(QKeyEvent*)> controller) {
    this->controller = controller;
}

void UIOverlay::start() {
    timerID = startTimer(1000 / UIOverlay::FRAMERATE);
}

void UIOverlay::terminate() {
    std::cout << "Destroying assets" << std::endl;
    NodeSprite::destroyAssets();
}

void UIOverlay::selectNode(const std::pair<int, int>& position) {
    this->nodesprites.at(position)->select();
}

void UIOverlay::deselectNode(const std::pair<int, int>& position) {
    this->nodesprites.at(position)->unselect();
}

void UIOverlay::highlightNode(const std::pair<int, int>& position) {
    this->nodesprites.at(position)->highlight();
}

void UIOverlay::setNodeIcons(const std::pair<int, int>& position,
                             const std::vector<std::shared_ptr<QIcon>>& icons) {
    this->nodesprites.at(position)->setIcons(icons);
}

void UIOverlay::deselectAllNodes() {
    for (auto nodesprite : this->nodesprites)
        nodesprite.second->unselect();
}

void UIOverlay::resetAllNodeIcons() {
    std::vector<std::shared_ptr<QIcon>> empty;
    for (auto nodesprite : this->nodesprites)
        nodesprite.second->setIcons(empty);
}

std::pair<int, int> UIOverlay::getResolution() {
    return std::make_pair(this->properties.width, this->properties.height);
}

void UIOverlay::render(QPainter& painter) {
    for (auto& map : this->nodesprites)
        map.second->render(this->properties, painter);
}