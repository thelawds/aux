//
// Created by miserable on 10.02.2022.
//

#ifndef AUX_PESUFFIXTREE_H
#define AUX_PESUFFIXTREE_H

#include <utility>

#include "TreeNode.h"

namespace aux::ir::ast {

    struct PeSuffixTree : TreeNode {

        std::shared_ptr<TreeNode> expression;

        std::shared_ptr<tokens::TokenIdentifier> identifier;

        explicit PeSuffixTree(std::shared_ptr<TreeNode> expression) : expression(std::move(expression)) {}

        explicit PeSuffixTree(std::shared_ptr<tokens::TokenIdentifier> identifier) : identifier(std::move(identifier)) {}

    };
}



#endif //AUX_PESUFFIXTREE_H
