//
// Created by miserable on 10.02.2022.
//

#ifndef AUX_ARITHMETICLOGICALEXPRESSIONTREE_H
#define AUX_ARITHMETICLOGICALEXPRESSIONTREE_H

#include <utility>

#include "TreeNode.h"
#include "../tokens/Token.h"

namespace aux::ir::ast {

    struct ArithmeticLogicalExpressionTree : TreeNode {
        std::shared_ptr<aux::ir::tokens::TokenOperator> op;

        ArithmeticLogicalExpressionTree(
                const std::shared_ptr<TreeNode> &left,
                const std::shared_ptr<TreeNode> &right,
                std::shared_ptr<aux::ir::tokens::TokenOperator> op
        ) : TreeNode(left, right), op(std::move(op)) {}

    };

}


#endif //AUX_ARITHMETICLOGICALEXPRESSIONTREE_H
