//
// Created by miserable on 10.02.2022.
//

#ifndef AUX_VARTREE_H
#define AUX_VARTREE_H

#include <utility>

#include "TreeNode.h"
#include "PeSuffixTree.h"

namespace aux::ir::ast {

    struct VarTree : TreeNode {
        std::shared_ptr<tokens::TokenIdentifier> identifier;
        std::vector<std::shared_ptr<PeSuffixTree>> peSuffix;
        std::shared_ptr<TreeNode> exp;

        explicit VarTree(std::shared_ptr<tokens::TokenIdentifier> identifier) : identifier(std::move(identifier)) {}

        explicit VarTree(std::shared_ptr<TreeNode> exp) : exp(std::move(exp)) {}

        inline void addPeSuffix(const std::shared_ptr<PeSuffixTree>& ptr) {
            peSuffix.push_back(ptr);
        }

    };

}

#endif //AUX_VARTREE_H
