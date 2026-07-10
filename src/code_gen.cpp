#include "../headers/code_gen.h"

void CodeGenerator::eval(ASTNode* n){
    switch (n->node_rule){
    case ASTNode::NodeRule::PROG:{
    
        break;
    }
    case ASTNode::NodeRule::MAINDECL:{
    
        break;
    }
    case ASTNode::NodeRule::CLASSDECL:{
    
        break;
    }
    case ASTNode::NodeRule::VARDECL:{
    
        break;
    }
    case ASTNode::NodeRule::METHODDECL:{
    
        break;
    }
    case ASTNode::NodeRule::COMMANDDECL:{
    
        break;
    }
    case ASTNode::NodeRule::ASSIGNDECL:{
    
        break;
    }
    case ASTNode::NodeRule::IFELSEDECL:{
    
        break;
    }
    case ASTNode::NodeRule::WHILEDECL:{
    
        break;
    }
    case ASTNode::NodeRule::PRINTLN:{
    
        break;
    }
    case ASTNode::NodeRule::ANDEXPR:{
    
        break;
    }
    case ASTNode::NodeRule::RELEXPR:{
    
        break;
    }
    case ASTNode::NodeRule::ADDEXPR:{
    
        break;
    }
    case ASTNode::NodeRule::MULDIVEXPR:{
    
        break;
    }
    case ASTNode::NodeRule::NEGATEEXPR:{
    
        break;
    }
    case ASTNode::NodeRule::PRIMARYACCESSEXPR:{
    
        break;
    }
    case ASTNode::NodeRule::PRIMARYEXPR:{
    
        break;
    }
    case ASTNode::NodeRule::TRUEFALSELITERAL:{
    
        break;
    }
    case ASTNode::NodeRule::NUMLITERAL:{
    
        break;
    }
    case ASTNode::NodeRule::IDLITERAL:{
        auto id_node = dynamic_cast<node_types::IdLiteral*>(n);
        
        break;
    }
    case ASTNode::NodeRule::THISEXPR:{
    
        break;
    }
    case ASTNode::NodeRule::NEWOBJEXPR:{
    
        break;
    }
    case ASTNode::NodeRule::NEWARRAYEXPR:{
    
        break;
    }
    default: {
    
        break;
    }
    }
}