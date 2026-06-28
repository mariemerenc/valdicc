#include "../headers/AST.h"
#include <sstream>

using namespace node_types;

/**
 * @brief Returns the direct children of any ASTNode, dispatching on its
 * node_rule tag and dynamic_cast-ing to the concrete type to reach its
 * fields.
 * 
 * This function is the single place that knows how each concrete node
 * type is shaped. It replaces a virtual getChildren() member on every
 * node class: instead of each class describing its own children, this
 * free function holds a big switch over every NodeRule and reaches into
 * the matching concrete class's public members.
 * 
 * @param node Node whose children should be listed. May be nullptr.
 * @return vector<ASTNode*> Direct children, in source order. Raw,
 *         non-owning pointers -- the unique_ptrs inside each node remain
 *         the sole owners.
 */
static vector<ASTNode*> get_children_for(ASTNode* node){
    vector<ASTNode*> filhos = {};

    if(node == nullptr){
        return filhos;
    }

    switch(node->node_rule){

        case ASTNode::NodeRule::PROG: {
            auto* n = dynamic_cast<ProgNode*>(node);
            if(n->main_decl != nullptr){
                filhos.push_back(n->main_decl.get());
            }
            for(auto& c : n->class_decl){
                filhos.push_back(c.get());
            }
            break;
        }

        case ASTNode::NodeRule::MAINDECL: {
            auto* n = dynamic_cast<MainDecl*>(node);
            for(auto& c : n->commands){
                filhos.push_back(c.get());
            }
            break;
        }

        case ASTNode::NodeRule::CLASSDECL: {
            auto* n = dynamic_cast<ClassDecl*>(node);
            for(auto& v : n->variables){
                filhos.push_back(v.get());
            }
            for(auto& m : n->methods){
                filhos.push_back(m.get());
            }
            break;
        }

        case ASTNode::NodeRule::VARDECL: {
            break;
        }

        case ASTNode::NodeRule::METHODDECL: {
            auto* n = dynamic_cast<MethodDecl*>(node);
            for(auto& p : n->param_list){
                filhos.push_back(p.get());
            }
            for(auto& c : n->commands_list){
                filhos.push_back(c.get());
            }
            if(n->return_expr != nullptr){
                filhos.push_back(n->return_expr.get());
            }
            break;
        }

        case ASTNode::NodeRule::COMMANDDECL: {
            auto* n = dynamic_cast<CommandDecl*>(node);
            if(n->command != nullptr){
                filhos.push_back(n->command.get());
            }
            break;
        }

        case ASTNode::NodeRule::ASSIGNDECL: {
            auto* n = dynamic_cast<AssignDecl*>(node);
            if(n->is_array && n->index_expr != nullptr){
                filhos.push_back(n->index_expr.get());
            }
            if(n->rhs != nullptr){
                filhos.push_back(n->rhs.get());
            }
            break;
        }

        case ASTNode::NodeRule::IFELSEDECL: {
            auto* n = dynamic_cast<IfElseDecl*>(node);
            if(n->if_exp != nullptr){
                filhos.push_back(n->if_exp.get());
            }
            for(auto& c : n->command_list){
                filhos.push_back(c.get());
            }
            if(n->has_else){
                for(auto& c : n->else_command_list){
                    filhos.push_back(c.get());
                }
            }
            break;
        }

        case ASTNode::NodeRule::WHILEDECL: {
            auto* n = dynamic_cast<WhileDecl*>(node);
            if(n->while_exp != nullptr){
                filhos.push_back(n->while_exp.get());
            }
            for(auto& c : n->command_list){
                filhos.push_back(c.get());
            }
            break;
        }

        case ASTNode::NodeRule::PRINTLN: {
            auto* n = dynamic_cast<PrintLn*>(node);
            if(n->print_exp != nullptr){
                filhos.push_back(n->print_exp.get());
            }
            break;
        }

        case ASTNode::NodeRule::ANDEXPR: {
            auto* n = dynamic_cast<AndExpr*>(node);
            if(n->lhs != nullptr) filhos.push_back(n->lhs.get());
            if(n->rhs != nullptr) filhos.push_back(n->rhs.get());
            break;
        }

        case ASTNode::NodeRule::RELEXPR: {
            auto* n = dynamic_cast<RelExpr*>(node);
            if(n->lhs != nullptr) filhos.push_back(n->lhs.get());
            if(n->rhs != nullptr) filhos.push_back(n->rhs.get());
            break;
        }

        case ASTNode::NodeRule::ADDEXPR: {
            auto* n = dynamic_cast<AddExpr*>(node);
            if(n->lhs != nullptr) filhos.push_back(n->lhs.get());
            if(n->rhs != nullptr) filhos.push_back(n->rhs.get());
            break;
        }

        case ASTNode::NodeRule::MULDIVEXPR: {
            auto* n = dynamic_cast<MulDivExpr*>(node);
            if(n->lhs != nullptr) filhos.push_back(n->lhs.get());
            if(n->rhs != nullptr) filhos.push_back(n->rhs.get());
            break;
        }

        case ASTNode::NodeRule::NEGATEEXPR: {
            auto* n = dynamic_cast<NegateExpr*>(node);
            if(n->lhs != nullptr){
                filhos.push_back(n->lhs.get());
            }
            break;
        }

        case ASTNode::NodeRule::PRIMARYACCESSEXPR: {
            auto* n = dynamic_cast<PrimaryAccessExpr*>(node);
            if(n->lhs != nullptr){
                filhos.push_back(n->lhs.get());
            }
            for(auto& a : n->access_expr){
                filhos.push_back(a.get());
            }
            for(auto& a : n->list_expression){
                filhos.push_back(a.get());
            }
            break;
        }

        case ASTNode::NodeRule::PRIMARYEXPR: {
            auto* n = dynamic_cast<PrimaryExpr*>(node);
            if(n->expr != nullptr){
                filhos.push_back(n->expr.get());
            }
            break;
        }

        case ASTNode::NodeRule::TRUEFALSELITERAL: {
            break;
        }

        case ASTNode::NodeRule::NUMLITERAL: {
            break;
        }

        case ASTNode::NodeRule::IDLITERAL: {
            break;
        }

        case ASTNode::NodeRule::THISEXPR: {
            break;
        }

        case ASTNode::NodeRule::NEWOBJEXPR: {
            break;
        }

        case ASTNode::NodeRule::NEWARRAYEXPR: {
            auto* n = dynamic_cast<NewArrayExpr*>(node);
            if(n->size_expr != nullptr){
                filhos.push_back(n->size_expr.get());
            }
            break;
        }

        default: {
            break;
        }
    }

    return filhos;
}

/**
 * @brief Recursively renders a node and its subtree into 'out'.
 * 
 * @param node Node to render. If nullptr, prints a placeholder and returns.
 * @param prefix Accumulated indentation string for this depth (grows by
 *               "    " or "│   " for each ancestor, depending on whether
 *               that ancestor was the last child at its own depth).
 * @param is_last Whether 'node' is the last child of its parent (controls
 *                which branch glyph -- ├─ or └── is used, and what prefix
 *                its own children should inherit).
 * @param out Stream the tree text is written into.
 */
static void print_node(ASTNode* node, const std::string& prefix, bool is_last, std::ostream& out){
    if(node == nullptr){
        out << prefix << (is_last ? "└─ " : "├─ ") << "(null)\n";
        return;
    }

    out << prefix << (is_last ? "└─ " : "├─ ") << node->to_string() << "\n";

    vector<ASTNode*> children = get_children_for(node);
    std::string child_prefix = prefix + (is_last ? "    " : "│   ");

    for(size_t i = 0; i < children.size(); i++){
        bool child_is_last = (i + 1 == children.size());
        print_node(children[i], child_prefix, child_is_last, out);
    }
}

string AST::print_tree(){
    std::ostringstream out;

    if(tree_root == nullptr){
        out << "(empty tree)\n";
        return out.str();
    }

    out << tree_root->to_string() << "\n";

    vector<ASTNode*> children = get_children_for(tree_root.get());
    for(size_t i = 0; i < children.size(); i++){
        bool child_is_last = (i + 1 == children.size());
        print_node(children[i], "", child_is_last, out);
    }

    return out.str();
}