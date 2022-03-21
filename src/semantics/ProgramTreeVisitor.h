//
// Created by miserable on 19.03.2022.
//

#ifndef AUX_PROGRAMTREEVISITOR_H
#define AUX_PROGRAMTREEVISITOR_H

namespace aux::ir::program_tree {

    namespace expression {
        struct NilTerm;
        struct ExpressionTerm;
        struct ArithmeticExpression;
    }

    namespace statement {
        struct ChunkTree;
        struct IdentifierVariableReferenceTree;
        struct AssignmentStatement;
    }

}

namespace aux::semantics {

    struct ProgramTreeVisitor {

        /**
         * @Expressions
         */

        virtual void visitNilTerm(aux::ir::program_tree::expression::NilTerm *) = 0;

        virtual void visitExpressionTerm(aux::ir::program_tree::expression::ExpressionTerm *) = 0;

        virtual void visitArithmeticExpression(aux::ir::program_tree::expression::ArithmeticExpression *) = 0;

        /**
         * @Statements
         */

        virtual void visitChunkTree(aux::ir::program_tree::statement::ChunkTree *) = 0;

        virtual void visitIdentifierVariableReferenceTree(
                aux::ir::program_tree::statement::IdentifierVariableReferenceTree *
        ) = 0;

        virtual void visitAssignmentStatement(aux::ir::program_tree::statement::AssignmentStatement *) = 0;

    };

}


#endif //AUX_PROGRAMTREEVISITOR_H
