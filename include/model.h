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

#include <memory>
#include <unordered_map>
#include <algorithm>
#include <vector>

#include "util.h"
#include "node.h"

class Model {
  public:
    typedef std::pair<util::Command, util::vec2i> command_position;
    // Populate node tree, allPaths is a vector of diffent paths,
    // current node will be root
    Model(std::shared_ptr<std::vector<command_position>>
          allPaths) :
        currentPosition(this->getRootPosition()) {
        if (allPaths->size() == 0 && (*allPaths)[0].second.size() > 0)
            return;
            
        std::pair<int, int> root_node = this->getRootPosition();
        std::shared_ptr<Node<util::Command>> root(new Node<util::Command>);
        
        // For every path in the paths
        for (auto& path : *allPaths) {
            // Make sure that the path is defined
            if (path.second.size() == 0)
                throw std::runtime_error("Path is empty!");
            std::pair<int, int> should_be_root = path.second[0];
            
            // The first node of every path should be the root
            if (should_be_root != root_node)
                throw std::runtime_error("Path does not begin with root node!");
                
            std::pair<int, int> lastPosition = root_node;
            
            std::shared_ptr<Node<util::Command>> curr = root;
            // For every node in the path
            for (auto& node : path.second) {
                std::pair<int, int> delta = node - lastPosition;
                lastPosition = node;
                if (delta == std::make_pair<int, int>(0, 0))
                    continue;
                else {
                    // Get the move between the two nodes
                    std::string direction = getDeltaDirection(delta);
                    std::shared_ptr<Node<util::Command>> child;
                    if (!curr->hasElement(direction))
                        child = std::shared_ptr<Node<util::Command>>
                                (new Node<util::Command>(curr));
                    else
                        child = curr->getElement(direction);
                    curr->setElement(direction, child);
                    curr = child;
                }
            }
            curr->setData(path.first);
        }
        this->currentNode = root;
    }
    
    // Constructor to return subtree model
    Model(std::shared_ptr<Node<util::Command>> root, std::pair<int, int> position) :
        currentNode(root), currentPosition(position) {
    }
    
    // Returns Model if valid, otherwise nulltpr
    std::shared_ptr<Model> select(const std::string& direction) const;
    
    // Gets the position of the root node
    std::pair<int, int> getRootPosition() const;
    
    // Gets current position
    std::pair<int, int> getCurrentPosition() const;
    
    // If we have a leaf selected, gets
    // the leaf
    std::shared_ptr<util::Command> getCommand() const;
    
    // Get list of commands that result from going in a
    // certain direction
    std::shared_ptr<std::vector<util::Command>> getCommandsInDirection(
                const std::string& direction) const;
                
    // Returns the path of nodes that have been selected
    std::shared_ptr<util::vec2i> getPath() const;
  private:
    std::shared_ptr<Node<util::Command>> currentNode;
    std::pair<int, int> currentPosition;
};