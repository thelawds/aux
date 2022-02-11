//
// Created by miserable on 10.02.2022.
//

#ifndef AUX_TREENODE_H
#define AUX_TREENODE_H

#include <memory>
#include <utility>
#include "../tokens/Token.h"

namespace aux::ir::ast {

    struct TreeNode {
        std::shared_ptr<TreeNode> left;
        std::shared_ptr<TreeNode> right;

        TreeNode(std::shared_ptr<TreeNode> left, std::shared_ptr<TreeNode> right)
                : left(std::move(left)), right(std::move(right)) {}

        TreeNode() = default;
    };

}


#endif //AUX_TREENODE_H
