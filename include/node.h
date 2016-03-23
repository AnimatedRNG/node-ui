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
#include <cstdlib>

#include "util.h"

template <class T>
class Node {
  public:
    typedef std::shared_ptr<Node <T>> node_ptr;
    typedef std::vector<std::shared_ptr<Node <T>>> child_vector;
    typedef std::unordered_map<std::string, child_vector> collapsed_children;
    
    Node() : Node(nullptr) { }
    
    Node(std::shared_ptr<Node<T>> parent) : Node(nullptr, parent) { }
    
    Node(std::shared_ptr<T> data_ptr, node_ptr parent) :
        children(),
        parent(parent),
        data(data_ptr),
        ID((static_cast<long>(rand()) << (sizeof(int) * 8)) |
           rand()) {
    }
    
    bool isLeaf() {
        if (children.count("r_") || children.count("ur") || children.count("u_") ||
                children.count("ul") || children.count("l_") ||
                children.count("ld") || children.count("d_") || children.count("dr"))
            return false;
        else
            return true;
    }
    
    bool isCommand() const {
        return false;
    }
    
    std::shared_ptr<std::vector<std::pair<std::string, node_ptr>>> unpack() const {
        std::vector<std::pair<std::string, node_ptr>> unpackedChildren;
        for (auto child : this->children) {
            unpackedChildren.push_back(std::make_pair(child.first, child.second));
        }
        return std::make_shared<std::vector<std::pair<std::string, node_ptr>>>
               (unpackedChildren);
    }
    
    std::vector<node_ptr> getChildrenList() const {
        std::vector<node_ptr> childList;
        for (auto child : children)
            childList.push_back(child.second);
        return childList;
    }
    
    node_ptr getParent() const {
        return parent;
    }
    
    void setData(T data) {
        this->data = std::make_shared<T>(data);
    }
    
    node_ptr getElement(const std::string& name) {
        return children[name];
    }
    
    void setElement(const std::string& element,
                    const std::shared_ptr<Node<T>>& value) {
        children[element] = value;
    }
    
    void setElement(const std::string& element,
                    const Node<T>& value) {
        children[element] = std::make_shared<Node<T>>(value);
    }
    
    bool hasElement(const std::string& element) {
        return this->children.count(element) > 0;
    }
    
    bool operator==(const Node<T>& other) const {
        return ID == other.ID;
    }
    
    bool operator!=(const Node<T>& other) const {
        return !(*this == other);
    }
    
    // Get map which contains vectors of all the children of the respective node, flattened into a list
    std::shared_ptr<collapsed_children> getCollapsedPossibilities() const {
        std::shared_ptr<collapsed_children> collapsedRules(new collapsed_children);
        auto unpackedChildren = * (this->unpack());
        for (auto map : unpackedChildren) {
            std::vector<node_ptr> collapsedChildrenVec;
            if (map.second != nullptr)
                collapsePossibilities(map.second, collapsedChildrenVec);
            (*collapsedRules)[map.first] = std::move(collapsedChildrenVec);
        }
        return collapsedRules;
    }
    
    std::unordered_map<std::string, node_ptr> children;
    std::shared_ptr<Node<T>> parent;
    std::shared_ptr<T> data;
    long ID;
  private:
    void collapsePossibilities(const node_ptr& node,
                               std::vector<node_ptr>& output) const {
        output.clear();
        if (node->isCommand()) {
            output.push_back(node);
            return;
        }
        for (auto& child : node->getChildrenList()) {
            if (!((*child).isCommand())) {
                std::vector<node_ptr> additionalOutput;
                collapsePossibilities(child, additionalOutput);
                std::copy(additionalOutput.begin(), additionalOutput.end(),
                          std::back_inserter(output));
            } else
                output.push_back(child);
        }
    }
};

template <>
inline bool Node<util::Command>::isCommand() const {
    return this->data != nullptr;
}

template <class T>
inline std::ostream& operator<<(std::ostream& strm, Node<T> const& node) {
    if (node.data != nullptr)
        strm << "Data: " << *(node.data);
    else
        strm << "No data found";
    for (auto child : *node.unpack()) {
        if (child.second != nullptr) {
            strm << "\t" << child.first << ": " << *(child.second);
            strm << "\n";
        }
    }
    return strm;
}

inline std::string getDeltaDirection(const std::pair<int, int>& delta) {
    if (delta == std::make_pair(1, 0))
        return "r_";
    else if (delta == std::make_pair(1, -1))
        return "ur";
    else if (delta == std::make_pair(1, 1))
        return "dr";
    else if (delta == std::make_pair(0, -1))
        return "u_";
    else if (delta == std::make_pair(0, 1))
        return "d_";
    else if (delta == std::make_pair(-1, 0))
        return "l_";
    else if (delta == std::make_pair(-1, -1))
        return "ul";
    else if (delta == std::make_pair(-1, 1))
        return "dl";
    else
        return "Invalid delta!";
}

inline std::pair<int, int> getDelta(const std::string& direction) {
    if (direction == "r_")
        return std::make_pair(1, 0);
    else if (direction == "ur")
        return std::make_pair(1, -1);
    else if (direction == "dr")
        return std::make_pair(1, 1);
    else if (direction == "u_")
        return std::make_pair(0, -1);
    else if (direction == "d_")
        return std::make_pair(0, 1);
    else if (direction == "l_")
        return std::make_pair(-1, 0);
    else if (direction == "ul")
        return std::make_pair(-1, -1);
    else if (direction == "dl")
        return std::make_pair(-1, 1);
    else
        return std::make_pair(0, 0);
}