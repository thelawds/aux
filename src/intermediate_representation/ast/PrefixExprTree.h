//
// Created by miserable on 10.02.2022.
//

#ifndef AUX_PREFIXEXPRTREE_H
#define AUX_PREFIXEXPRTREE_H

#include "TreeNode.h"
#include <utility>
#include <vector>

namespace aux::ir::ast {

    enum SuffixType{
        FUNC_CALL,
        PE
    };

    struct PrefixExprTree : TreeNode {
        std::shared_ptr<tokens::TokenIdentifier> identifier;
        std::vector<std::shared_ptr<TreeNode>> peFuncCallSuffix;
        std::vector<SuffixType> suffixes;
        std::shared_ptr<TreeNode> exp;

        explicit PrefixExprTree(std::shared_ptr<tokens::TokenIdentifier> identifier)
                : identifier(std::move(identifier)) {}

        explicit PrefixExprTree(std::shared_ptr<TreeNode> exp) : exp(std::move(exp)) {}

        inline void addPeOrFuncCallSuffix(const std::shared_ptr<PeSuffixTree>& treeNode){
            peFuncCallSuffix.push_back(treeNode);
            suffixes.push_back(PE);
        }

        inline void addPeOrFuncCallSuffix(const std::shared_ptr<FunctionCallTree>& treeNode){
            peFuncCallSuffix.push_back(treeNode);
            suffixes.push_back(FUNC_CALL);
        }

    };
}


#endif //AUX_PREFIXEXPRTREE_H
