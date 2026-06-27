#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <string>

namespace{ using std::vector, std::string, std::unique_ptr;}
/**
 * @class ASTNode
 * @brief Base class for every node in the Abstract Syntax Tree. 
 * 
 * Stores the grammar rule that produced the node and provides the virtual
 * interface used to visualize the tree (to_string) and traverse it (getChildren);
 */
class ASTNode{
public:
    /** @brief Enumerates every grammar production representable as an AST node.*/
    enum class NodeRule{
        PROG,
        MAINDECL,
        CLASSDECL,
        VARDECL,
        METHODDECL,
        COMMANDDECL,
        ASSIGNDECL,
        IFELSEDECL,
        WHILEDECL,
        PRINTLN,
        ANDEXPR,
        RELEXPR,
        ADDEXPR,
        MULDIVEXPR,
        NEGATEEXPR,
        PRIMARYACCESSEXPR,
        PRIMARYEXPR,
        TRUEFALSELITERAL,
        NUMLITERAL,
        IDLITERAL,
        THISEXPR,
        NEWOBJEXPR,
        NEWARRAYEXPR,
    } node_rule;

    /** @brief Returns a one-line text label for this node.*/
    virtual string to_string() {return "";};
    /** @brief Returns pointers to the node's direct children*/
    virtual vector<ASTNode*> getChildren() {return {};};
};

/**
 * @class ExprNode 
 * @brief Class for all expression nodes.
 * 
 * Adds the static type of the expression and a slot for its evaluated
 * value, to be checked and populated during semantic analysis.
 */
class ExprNode : public ASTNode{
public:
    enum class ExprType{
        INT, BOOL, ID, INT_ARR
    }; 
protected:
    ExprType type;
    union DataVal{
        double floatval;
        int64_t intval;
        bool boolval;
    } val;
    size_t n_array; //if 0, it's a plain value. other wise is an n-dimensional array
};



namespace node_types{
    /**
     * @class ProgNode
     * @brief Root node.
     * 
     */
    class ProgNode : public ASTNode{
        unique_ptr<ASTNode> main_decl;
        vector<unique_ptr<ASTNode>> class_decl;
    public:
        ProgNode(unique_ptr<ASTNode> m, vector<unique_ptr<ASTNode>> &&c){
            main_decl = std::move(m);
            class_decl = std::move(c);
            this->node_rule = NodeRule::PROG;
        }
    };
     /**
     * @class MainDecl
     * @brief The main class and its 'main(String[]) method body.
     * 
     */
    class MainDecl : public ASTNode{
        string main_class_id;
        string args_var_id;
        vector<unique_ptr<ASTNode>> commands;
    public:
        MainDecl(const string &m_id, const string &a_id, vector<unique_ptr<ASTNode>> &&c){
            main_class_id = m_id;
            args_var_id = a_id;
            commands = std::move(c);
            this->node_rule = NodeRule::MAINDECL;
        }

        string to_string(){
            return "MAIN(class = " + main_class_id + ", args = " + args_var_id + ")";
        }

        vector<ASTNode*> getChildren(){
            vector<ASTNode*> filhos = {};
            for(auto& c : commands){
                filhos.push_back(c.get());
            }
            
            return filhos;
        }
    };

     /**
     * @class ClassDecl
     * @brief A class declaration, possibly with inheritance.
     * 
     */
    class ClassDecl : public ASTNode{
        string class_id;
        vector<unique_ptr<ASTNode>> variables;
        vector<unique_ptr<ASTNode>> methods;
        bool extends;
        string inherit_id;
    public:
        ClassDecl(const string &c_id, vector<unique_ptr<ASTNode>> &&v, vector<unique_ptr<ASTNode>> &&m, bool e, const string &i_id){
            class_id = c_id;
            variables = std::move(v);
            methods = std::move(m);
            extends = e;
            inherit_id = i_id;
            this->node_rule = NodeRule::CLASSDECL;
        }
    };
    
     /**
     * @class VarDecl
     * @brief A variable declaration.
     * 
     */
    class VarDecl : public ASTNode{
        string var_id;
        string var_type;
        bool is_array;
    public:
        VarDecl(const string &v_id, const string &v_type){
            var_id = v_id;
            var_type = v_type;
            if(var_type.back() == ']') is_array = true;
            else is_array = false;
            
            this->node_rule = NodeRule::VARDECL;
        }
    };

     /**
     * @class MethodDecl
     * @brief A method declaration (signature, params, body, and return).
     * 
     */
    class MethodDecl : public ASTNode{
        string method_id;
        string methodtype;
        vector<unique_ptr<ASTNode>> param_list;
        vector<unique_ptr<ASTNode>> commands_list;
        unique_ptr<ExprNode> return_expr;
    public:
        MethodDecl(const string &met_id, const string& met_type,vector<unique_ptr<ASTNode>> &&p_list, vector<unique_ptr<ASTNode>> &&c_list, unique_ptr<ExprNode> r_expr){
            method_id = met_id;
            methodtype = met_type;
            param_list = std::move(p_list);
            commands_list = std::move(c_list);
            return_expr = std::move(r_expr);
            this->node_rule = NodeRule::METHODDECL;
        }

        string to_string(){
            return "METHOD(" + method_id + " : " + methodtype + ")";
        }

        vector<ASTNode*> getChildren(){
            vector<ASTNode*> filhos = {};
            for(auto& p : param_list){
                filhos.push_back(p.get());
            }
            for(auto& c : commands_list){
                filhos.push_back(c.get());
            }
            if(return_expr != nullptr){
                filhos.push_back(return_expr.get());
            }

            return filhos;
        }
    };

     /**
     * @class CommandDecl
     * @brief honestamente não sei, não usei...
     * 
     */
    class CommandDecl : public ASTNode{
        unique_ptr<ASTNode> command; 
    public:
        CommandDecl(unique_ptr<ASTNode> c){
            command = std::move(c);
            this->node_rule = NodeRule::COMMANDDECL;
        }
    };

     /**
     * @class AssignDecl
     * @brief An assignment command.
     * 
     */
    class AssignDecl : public ASTNode{
        string lhs_id;
        bool is_array;
        unique_ptr<ExprNode> index_expr;
        unique_ptr<ExprNode> rhs;
    public:
        AssignDecl(const string &l_id, bool arr, unique_ptr<ExprNode> i_expr, unique_ptr<ExprNode> r_expr){
            lhs_id = l_id;
            is_array = arr;
            index_expr = std::move(i_expr);
            rhs = std::move(r_expr);
            this->node_rule = NodeRule::ASSIGNDECL;
        }
    };

     /**
     * @class IfElseDecl
     * @brief An if/else command with { command(s) }.
     * 
     */
    class IfElseDecl : public ASTNode {
        unique_ptr<ExprNode> if_exp;
        vector<unique_ptr<ASTNode>> command_list;
        bool has_else;
        vector<unique_ptr<ASTNode>> else_command_list;
    public:
        IfElseDecl(unique_ptr<ExprNode> i_exp, vector<unique_ptr<ASTNode>> &&c_list, bool h_else, vector<unique_ptr<ASTNode>> &&e_c_list){
            if_exp = std::move(i_exp);
            command_list = std::move(c_list);
            has_else = h_else;
            else_command_list = std::move(e_c_list);
            this->node_rule = NodeRule::IFELSEDECL;
        }
    };

     /**
     * @class WhileDecl
     * @brief A while loop with { command(s) }.
     * 
     */
    class WhileDecl : public ASTNode{
        unique_ptr<ExprNode> while_exp;
        vector<unique_ptr<ASTNode>> command_list;
    public:
        WhileDecl(unique_ptr<ExprNode> w_exp, vector<unique_ptr<ASTNode>> &&c_list){
            while_exp = std::move(w_exp);
            command_list = std::move(c_list);
            this->node_rule = NodeRule::WHILEDECL;
        }
    };

     /**
     * @class PrintLn
     * @brief System.out.println(exp) command.
     * 
     */
    class PrintLn : public ASTNode{
        unique_ptr<ExprNode> print_exp;
    public:
        PrintLn(unique_ptr<ExprNode> p_expr){
            print_exp = std::move(p_expr);
            this->node_rule = NodeRule::PRINTLN;
        }
    };

     /**
     * @class AndExpr
     * @brief Logical AND expression. Result type is boolean.
     * 
     */
    class AndExpr : public ExprNode{
        unique_ptr<ExprNode> lhs;
        unique_ptr<ExprNode> rhs;
        bool val;
    public:
        AndExpr(unique_ptr<ExprNode> left, unique_ptr<ExprNode> right){
            lhs = std::move(left);
            rhs = std::move(right);
            val = false;
            type = ExprType::BOOL;
            this->node_rule = NodeRule::ANDEXPR;
        }
    };

     /**
     * @class RelExpr
     * @brief Relational comparison (greater than) expression.
     * Result type is boolean.
     * 
     */
    class RelExpr : public ExprNode{
        unique_ptr<ExprNode> lhs;
        unique_ptr<ExprNode> rhs;
        bool val;
    public:
        RelExpr(unique_ptr<ExprNode> left, unique_ptr<ExprNode> right){
            lhs = std::move(left);
            rhs = std::move(right);
            val = false;
            type = ExprType::BOOL;
            this->node_rule = NodeRule::RELEXPR;
        }

    };

     /**
     * @class AddExpr
     * @brief Additive expression (+ / -). Result type is int.
     * 
     */
    class AddExpr : public ExprNode{
    public:
        enum class Operation{
            SUM,
            SUB,
        } op;
    private:
        int64_t val;
        unique_ptr<ExprNode> lhs;
        unique_ptr<ExprNode> rhs;
    public:
        AddExpr(Operation operation, unique_ptr<ExprNode> left, unique_ptr<ExprNode> right){
            op = operation;
            lhs = std::move(left);
            rhs = std::move(right);
            val = 0;
            type = ExprType::INT;
            this->node_rule = NodeRule::ADDEXPR;
        }
    };

     /**
     * @class MulDivExpr
     * @brief Multiplicative expression (*). Result type is int.
     * 
     */
    class MulDivExpr : public ExprNode{
    public:
    enum class Operation{
            MUL,
            DIV,
        } op;
    private:
        int64_t val;
        unique_ptr<ExprNode> lhs;
        unique_ptr<ExprNode> rhs;
    public:
        MulDivExpr(Operation operation, unique_ptr<ExprNode> left, unique_ptr<ExprNode> right){
            op = operation;
            lhs = std::move(left);
            rhs = std::move(right);
            val = 0;
            type = ExprType::INT;
            this->node_rule = NodeRule::MULDIVEXPR;
        }
        
    };

     /**
     * @class NegateExpr
     * @brief Logical negation (!) expression. Result type is boolean.
     * 
     */
    class NegateExpr : public ExprNode{
        bool is_negated;
        unique_ptr<ExprNode> lhs;
    public:
        NegateExpr(bool negated, unique_ptr<ExprNode> left){
            is_negated = negated;
            lhs = std::move(left);
            type = ExprType::BOOL;
            this->node_rule = NodeRule::NEGATEEXPR;
        }
    };

     /**
     * @class PrimaryAccessExpr
     * @brief Postfix access on a primary expression (array index,
     * length or method call);
     * @note Psfexp in the described language syntax
     */
    class PrimaryAccessExpr : public ExprNode{
    public:
        enum class PEModifier {
            LENGTH,
            ARRAY_ACCESS,
            CLASS_CONSTRUCT,
            METHOD_CALL,
        };
    private:
        unique_ptr<ExprNode> lhs;
        vector<unique_ptr<ExprNode>> access_expr;
            
        PEModifier expr_kind;
        vector<unique_ptr<ExprNode>> list_expression;//expression list for the method call
    public:
        PrimaryAccessExpr(unique_ptr<ExprNode> left, PEModifier kind, vector<unique_ptr<ExprNode>> access, vector<unique_ptr<ExprNode>> list_expr = {}){
            lhs = std::move(left);
            expr_kind = kind;
            access_expr = std::move(access);
            list_expression = std::move(list_expr);
            type = ExprType::INT;
            this->node_rule = NodeRule::PRIMARYACCESSEXPR;
        }

        string to_string(){
            string kindStr = "";
            if(expr_kind == PEModifier::LENGTH){
                kindStr = "length";
            }
            else if(expr_kind == PEModifier::ARRAY_ACCESS){
                kindStr = "array[]";
            }
            else if(expr_kind == PEModifier::METHOD_CALL){
                kindStr = "method call";
            }
            else{
                kindStr = "new";
            }

            return "PrimaryAccess(" + kindStr + ")";
        }

        vector<ASTNode*> getChildren(){
            vector<ASTNode*> filhos = {lhs.get()};
            for(auto& ae : access_expr){
                filhos.push_back(ae.get());
            }
            for(auto& le : list_expression){
                filhos.push_back(le.get());
            }
            
            return filhos;
        }
    };

     /**
     * @class PrimaryExpr
     * @brief A parenthesized expression (EXP).
     * 
     */
    class PrimaryExpr : public ExprNode{
        unique_ptr<ExprNode> expr;
    public:
        PrimaryExpr(unique_ptr<ExprNode> inner){
            expr = std::move(inner);
            this->node_rule = NodeRule::PRIMARYEXPR;
        }
    };

     /**
     * @class TrueFalseLiteral
     * @brief A boolean literal (true or false).
     * 
     */
    class TrueFalseLiteral : public ExprNode{
        bool bool_val;
    public:
        TrueFalseLiteral(bool v){
            bool_val = v;
            type = ExprType::BOOL;
            this->node_rule = NodeRule::TRUEFALSELITERAL;
        }
    };

     /**
     * @class NumLiteral
     * @brief An integer literal.
     * 
     */
    class NumLiteral : public ExprNode{
        int64_t int_val;
    public:
        NumLiteral(int64_t v){
            int_val = v;
            type = ExprType::INT;
            this->node_rule = NodeRule::NUMLITERAL;
        }
    };

     /**
     * @class IdLiteral
     * @brief An identifier reference used as an expression.
     * 
     */
    class IdLiteral : public ExprNode{
        string id;
    public:
        IdLiteral(string identifier){
            id = std::move(identifier);
            type = ExprType::ID;
            this->node_rule = NodeRule::IDLITERAL;
        }
        
    };

     /**
     * @class ThisExpr
     * @brief The 'this' reference to the current object.
     * 
     */
    class ThisExpr : public ExprNode{
    public:
        ThisExpr(){
            type = ExprType::ID;
            this->node_rule = NodeRule::THISEXPR;
        }
    };
    
     /**
     * @class NewObjExpr
     * @brief Object construction 'new Obj()'.
     * 
     */
    class NewObjExpr : public ExprNode{
        string class_id;

    public:
        NewObjExpr(string id){
            class_id = std::move(id);
            type = ExprType::ID;
            this->node_rule = NodeRule::NEWOBJEXPR;
        }
    };

     /**
     * @class NewArrayExpr
     * @brief Array construction 'new int[TAM].
     * 
     */
    class NewArrayExpr : public ExprNode{
        unique_ptr<ExprNode> size_expr;

    public:
        NewArrayExpr(unique_ptr<ExprNode> size){
            size_expr = std::move(size);
            type = ExprType::INT_ARR;
            this->node_rule = NodeRule::NEWARRAYEXPR;
        }
    };
}

/**
 * @class AST
 * @brief
 * 
 */
class AST{
private:
    unique_ptr<ASTNode> tree_root;
public:
    AST(unique_ptr<ASTNode> p) : tree_root{std::move(p)} {}
    string print_tree();
};