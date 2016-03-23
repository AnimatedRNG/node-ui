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

#include <cxxtest/TestSuite.h>
#include "assert.h"
#include "util.h"
#include "node.h"

class NodeTestSuite : public CxxTest::TestSuite {
  public:
  
    NodeTestSuite() {}
    
    void setUp() {
    
    }
    
    void test_unpack() {
        Node<util::Command> node;
        Node<util::Command> node2;
        Node<util::Command> node3;
        util::Command testing;
        testing.name = "Testing";
        node.setElement("r_", node2);
        node.getElement("r_")->setData(testing);
        node.getElement("r_")->setElement("ur", node3);
        node.getElement("r_")->getElement("ur")->setData(testing);
        auto unpackedChildren = * (node.unpack());
        for (auto element : unpackedChildren) {
            if (element.second != nullptr)
                assert(element.first == "r_");
        }
        std::shared_ptr<Node<util::Command>::collapsed_children> collapsed =
            node.getCollapsedPossibilities();
        assert(!node.isLeaf());
        assert(node2.isLeaf());
        for (auto element : (*collapsed)) {
            std::cout << "(" << element.first << ", " << element.second.size() << ")" <<
                      std::endl;
            if (element.second.size() > 0) {
                assert(element.second.size() == 1);
                for (auto e : element.second) {
                    std::cout << "\t" << *e << std::endl;
                }
            }
        }
    }
};