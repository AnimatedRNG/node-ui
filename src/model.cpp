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
#include "node.h"
#include "model.h"

std::shared_ptr<Model> Model::select(const std::string& direction) const {
    if (this->currentNode->hasElement(direction))
        return std::make_shared<Model>(
                   Model(this->currentNode->getElement(direction),
                         getDelta(direction) + this->getCurrentPosition()));
    else
        return std::make_shared<Model>(Model(this->currentNode,
                                             this->getCurrentPosition()));
}

std::shared_ptr<Model> Model::selectParent() const {
    if (this->currentNode->parent != nullptr) {
        std::shared_ptr<Node<util::Command>> parent = this->currentNode->parent;
        std::pair<int, int> delta;
        auto unpackedChildren = *(currentNode->parent->unpack());
        for (auto& map : unpackedChildren) {
            if (map.second->ID == this->currentNode->ID) {
                delta = getDelta(map.first);
                break;
            }
        }
        return std::make_shared<Model>(Model(parent,
                                             this->getCurrentPosition() - delta));
    } else
        return std::make_shared<Model>(Model(this->currentNode,
                                             this->getCurrentPosition()));
}

std::pair<int, int> Model::getRootPosition() const {
    return {util::GRID_WIDTH / 2, util::GRID_HEIGHT / 2};
}

std::pair<int, int> Model::getCurrentPosition() const {
    return this->currentPosition;
}

std::shared_ptr<util::Command> Model::getCommand() const {
    if (this->currentNode->isLeaf())
        return this->currentNode->data;
    else
        return nullptr;
}

std::shared_ptr<std::vector<util::Command>> Model::getCommandsInDirection(
const std::string& direction) const {
    std::shared_ptr<std::vector<util::Command>> commands(new
            std::vector<util::Command>);
    // Assuming there exists a child in the intended direction,
    if (this->currentNode->hasElement(direction)) {
        // Get all the collapsed possibilities for the node
        std::shared_ptr <Node<util::Command>::collapsed_children> collapsed =
            this->currentNode->getCollapsedPossibilities();
        // And put the commands associated with the leaves in that direction
        // in a vector
        for (auto& collapsedChild : (*collapsed)[direction])
            commands->push_back(*(collapsedChild->data));
    }
    return commands;
}

std::shared_ptr<std::vector<std::string>> Model::getViableDirections() const {
    std::vector<std::string> possibilities;
    
    for (auto& map : this->currentNode->children)
        possibilities.push_back(map.first);
        
    return std::make_shared<std::vector<std::string>>(possibilities);
}

std::shared_ptr<util::vec2i> Model::getPath() const {
    Node<util::Command>::node_ptr curr = this->currentNode;
    std::pair<int, int> currPos = this->currentPosition;
    util::vec2i path;
    
    // Add the current location down the path
    path.push_back(currPos);
    
    // Iterate up the tree
    while (curr->parent != nullptr) {
        auto unpackedChildren = *(curr->parent->unpack());
        // For every child of this node's parents (including the node itself)
        for (auto map : unpackedChildren) {
            // If we have found ourselves in the children
            if (map.second->ID == curr->ID) {
                // Get the delta to get from the parent to us
                std::pair<int, int> delta = getDelta(map.first);
                // Alter our current position by the opposity of the delta
                currPos = currPos - delta;
                // Add the current position to the path
                path.push_back(currPos);
                // Set our current node to the parent
                curr = curr->parent;
            }
        }
    }
    
    // Path is currently in reverse
    std::reverse(path.begin(), path.end());
    return std::make_shared<util::vec2i>(path);
}

void Model::reset() {
    auto curr = this->currentNode;
    while (curr->parent != nullptr)
        curr = curr->parent;
    this->currentNode = curr;
    this->currentPosition = this->getRootPosition();
}