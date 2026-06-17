#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <string>

namespace{ using std::vector, std::string, std::unique_ptr;}
class ASTNode{
public:
    virtual string to_string() = 0;
};


class ExprNode : public ASTNode{
protected:
    enum class ExprType{
        INT, BOOL, ID, INT_ARR
    } type;
    void* val;
    size_t n_array; //if 0, it's a plain value. other wise is an n-dimensional array
};

class AST{
private:
    unique_ptr<ASTNode> tree_root;
public:
    string print_tree();
};

namespace node_types{
    class ProgNode : public ASTNode{
        unique_ptr<ASTNode> main_decl;
        vector<unique_ptr<ASTNode>> class_decl;
    };

    class MainDecl : public ASTNode{
        string main_class_id;
        string args_var_id;
        vector<unique_ptr<ASTNode>> commands;
    };

    class ClassDecl : public ASTNode{
        string class_id;
        vector<unique_ptr<ASTNode>> variables;
        vector<unique_ptr<ASTNode>> methods;
        bool extends;
        string inherit_id;
    };
    class VarDecl : public ASTNode{
        string var_id;
        string var_type;
        bool is_array;
    };
    class MethodDecl : public ASTNode{
        string method_id;
        vector<unique_ptr<ASTNode>> param_list;
        vector<unique_ptr<ASTNode>> commands_list;
        unique_ptr<ExprNode> return_expr;
    };
    class CommandDecl : public ASTNode{
        unique_ptr<ASTNode> command; 
    };
    class AssignDecl : public ASTNode{
        string lhs_id;
        bool is_array;
        unique_ptr<ExprNode> index_expr;
        unique_ptr<ExprNode> rhs;
    };
    class IfElseDecl : public ASTNode {
        unique_ptr<ExprNode> if_exp;
        vector<unique_ptr<ASTNode>> command_list;
        bool has_else;
        vector<unique_ptr<ASTNode>> else_command_list;
    };
    class WhileDecl : public ASTNode{
        unique_ptr<ExprNode> while_exp;
        vector<unique_ptr<ASTNode>> command_list;
    };
    class PrintLn : public ASTNode{
        unique_ptr<ExprNode> print_exp;
    };
    class AndExpr : public ExprNode{
        unique_ptr<ExprNode> lhs;
        unique_ptr<ExprNode> rhs;
        bool val;
        
    };
    class RelExpr : public ExprNode{
        unique_ptr<ExprNode> lhs;
        unique_ptr<ExprNode> rhs;
        bool val;

    };
    class AddExpr : public ExprNode{
        int64_t val;
        enum class Operation{
            SUM,
            SUB,
        } op;
        unique_ptr<ExprNode> lhs;
        unique_ptr<ExprNode> rhs;
    };
    class MulDivExpr : public ExprNode{
        int64_t val;
        enum class Operation{
            MUL,
            DIV,
        } op;
        unique_ptr<ExprNode> lhs;
        unique_ptr<ExprNode> rhs;
        
    };
    class NegateExpr : public ExprNode{
        bool is_negated;
        unique_ptr<ExprNode> lhs;
    };
    //Psfexp in the described language syntax
    class PrimaryAccessExpr : public ExprNode{
        unique_ptr<ExprNode> lhs;
        vector<unique_ptr<ExprNode>> access_expr;
        enum class PEModifier {
            LENGTH,
            ARRAY_ACCESS,
            CLASS_CONSTRUCT,
            METHOD_CALL,
        };
        PEModifier expr_kind;
        vector<unique_ptr<ExprNode>> list_expression;//expression list for the method call
    };
    class PrimaryExpr : public ExprNode{
        unique_ptr<ExprNode> expr;
    };
    class TrueFalseLiteral : public ExprNode{
        bool bool_val;
        TrueFalseLiteral(bool v){
            bool_val = v;
            type = ExprType::BOOL;
        }
    };
    class NumLiteral : public ExprNode{
        int64_t int_val;
        NumLiteral(int64_t v){
            int_val = v;
            type = ExprType::INT;
        }
    };
    class IdLiteral : public ExprNode{
        //has to have the symbol table implemented
    };
}