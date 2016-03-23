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
            assert(path.second.size() != 0);
            std::pair<int, int> should_be_root = path.second[0];
            
            // The first node of every path should be the root
            assert(should_be_root == root_node);
            
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
                    std::shared_ptr<Node<util::Command>> child(new Node<util::Command>
                                                      (curr));
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