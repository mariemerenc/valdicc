#pragma once

#include <memory>
#include <vector>
#include <string>

namespace{ using std::vector, std::string, std::unique_ptr;}
class ASTNode{
public:
    virtual string to_string() = 0;
};


class ExprNode : public ASTNode{};

class AST{
    unique_ptr<ASTNode> tree_root;
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

}