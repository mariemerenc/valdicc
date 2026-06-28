#include "../headers/AST.h"
#include <sstream>

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

    std::vector<ASTNode*> children = node->getChildren();
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

    std::vector<ASTNode*> children = tree_root->getChildren();
    for(size_t i = 0; i < children.size(); i++){
        bool child_is_last = (i + 1 == children.size());
        print_node(children[i], "", child_is_last, out);
    }

    return out.str();
}