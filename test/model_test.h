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
#include "model.h"

class ModelTestSuite : public CxxTest::TestSuite {
  public:
  
    ModelTestSuite() {}
    
    void setUp() {
    
    }
    
    void test_model() {
        std::vector<Model::command_position> paths;
        paths.push_back(std::make_pair(util::Command {"Example"}, util::vec2i{ {1, 1}, {1, 2}, {2, 2} }));
        paths.push_back(std::make_pair(util::Command {"Example2"}, util::vec2i{ {1, 1}, {1, 0}, {0, 0} }));
        Model model(
            std::make_shared<std::vector<Model::command_position>> (paths));
        std::shared_ptr<Model> child = model.select("u_");
        assert((model.getCurrentPosition() - child->getCurrentPosition() ==
                std::make_pair(0, 1)));
        DEBUG(child->select("l_")->getCommand()->name);
        assert(child->select("l_")->getCommand()->name == "Example2");
        assert((*(model.getCommandsInDirection("d_")))[0].name == "Example");
        assert((*(model.getCommandsInDirection("u_")))[0].name == "Example2");
        util::vec2i path = *(child->select("l_")->getPath());
        DEBUGARR(path);
    }
};