//
// Created by miserable on 19.03.2022.
//

#ifndef AUX_PROGRAMTREE_H
#define AUX_PROGRAMTREE_H

#include "util/Defines.h"
#include "semantics/ProgramTreeVisitor.h"
#include "intermediate_representation/Token.h"

#include <memory>
#include <vector>
#include <utility>

namespace aux::ir::program_tree {

    struct ProgramTree {

        virtual void accept(aux::semantics::ProgramTreeVisitor *visitor) = 0;
    };

    MARKER_INTERFACE struct ExpressionTree : virtual ProgramTree {};

    MARKER_INTERFACE struct StatementTree : virtual ProgramTree {};

    /**
     * <b> Expression Trees </b>
     */

    namespace expression {

        struct NilTerm : ExpressionTree {
            void accept(aux::semantics::ProgramTreeVisitor *visitor) override;
        };

        struct ExpressionTerm : ExpressionTree {
            enum TermType {
                IDENTIFIER,
                BOOLEAN,
                INTEGER,
                FLOAT,
                STRING_LITERAL
            };

            std::string value;
            TermType termType;

            explicit ExpressionTerm(std::string value, TermType termType);

            void accept(aux::semantics::ProgramTreeVisitor *visitor) override;

        };

        struct ArithmeticExpression : ExpressionTree {

            enum ArithmeticOperation {
                ADDITION,
                SUBTRACTION,
                MULTIPLICATION,
                DIVISION,
                FLOOR_DIVISION,
                MODULO,
                EXPONENTIATION
            };

            std::shared_ptr<ExpressionTree> left;
            std::shared_ptr<ExpressionTree> right;
            ArithmeticOperation operation;

            ArithmeticExpression(
                    const std::shared_ptr<ExpressionTree> &left,
                    const std::shared_ptr<ExpressionTree> &right,
                    aux::ir::tokens::Operator tokenOperator
            );

            ArithmeticExpression(
                    const std::shared_ptr<ExpressionTree> &left,
                    const std::shared_ptr<ExpressionTree> &right,
                    ArithmeticOperation operation
            );

            void accept(aux::semantics::ProgramTreeVisitor *visitor) override;

        private:
            static ArithmeticOperation convert(const aux::ir::tokens::Operator &tokenOperator);

        };

    }

    namespace statement {

        MARKER_INTERFACE struct VariableReferenceTree : virtual ProgramTree {};

        struct ChunkTree : StatementTree {
            std::vector<std::shared_ptr<StatementTree>> statements;

            void accept(aux::semantics::ProgramTreeVisitor *visitor) override;

            void append(const std::shared_ptr<StatementTree>& statementTree);

        };

        struct IdentifierVariableReferenceTree : VariableReferenceTree {
            std::string identifier;

            explicit IdentifierVariableReferenceTree(std::string identifier);

            void accept(aux::semantics::ProgramTreeVisitor *visitor) override;

        };

        struct AssignmentStatement : StatementTree {
            using Assignment = std::pair<std::shared_ptr<VariableReferenceTree>, std::shared_ptr<ExpressionTree>>;

            std::vector<Assignment> assignments;

            void append(const std::shared_ptr<VariableReferenceTree> &);
            void append(const std::shared_ptr<VariableReferenceTree> &, const std::shared_ptr<ExpressionTree> &);

            void accept(aux::semantics::ProgramTreeVisitor *visitor) override;
        };

    }

}

#endif //AUX_PROGRAMTREE_H
