//
// Created by miserable on 10.02.2022.
//

#ifndef AUX_FUNCTIONCALLTREE_H
#define AUX_FUNCTIONCALLTREE_H

#include <utility>

#include "TreeNode.h"
#include "ArgsTree.h"

namespace aux::ir::ast {

    struct FunctionCallTree : TreeNode {
        std::shared_ptr<TreeNode> prefixExp;
        std::shared_ptr<ArgsTree> args;
        std::shared_ptr<tokens::TokenIdentifier> identifier;

        FunctionCallTree(std::shared_ptr<TreeNode> prefixExp, std::shared_ptr<ArgsTree> args)
                : prefixExp(std::move(prefixExp)), args(std::move(args)) {}

        FunctionCallTree(
                std::shared_ptr<TreeNode> prefixExp,
                std::shared_ptr<ArgsTree> args,
                std::shared_ptr<tokens::TokenIdentifier> identifier
        ) : prefixExp(std::move(prefixExp)), args(std::move(args)), identifier(std::move(identifier)) {}

        FunctionCallTree(
                std::shared_ptr<ArgsTree> args,
                std::shared_ptr<tokens::TokenIdentifier> identifier
        ) : args(std::move(args)), identifier(std::move(identifier)) {}
    };

}

#endif //AUX_FUNCTIONCALLTREE_H
