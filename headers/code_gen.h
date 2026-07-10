#pragma once
#include "AST.h"
#include "environment.h"
#include "symbol_table.h"
#include <map>
#include <string>

class CodeGenerator{
    std::map<std::string, ASTNode*> labels;
    ASTNode* root_node;
    Environment env;
    size_t label_count;
    void eval(ASTNode* n);
    void generate();
public:
    std::string code;
    CodeGenerator(ASTNode* n, Environment e) : root_node{n}, env{e}{
        eval(n);
    }
};