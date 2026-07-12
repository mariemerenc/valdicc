#ifndef TAC_GENERATOR
#define TAC_GENERATOR

#include <vector>
#include <list>
#include "types.h"
#include "AST.h"


enum class oper{
    ADD,
    SUB,
    MULT,
    DIV,
    AND,
    OR,
    NOT,
    ACC,
    LEN,
    ASS,
    GOTO,
    GOTOIF,
    NUM,
    TRUEFALSE,
    ID,
    PARAM,
    CALL,
    ACCESS,
    PRINT,
    DEFINE,
    RETURN,
    NEWOBJ,
    NEWARR,
    THIS,
};

struct quad {

    
    oper opp;

    // THESE ARE PLACEHOLDERS.
    string arg1, arg2, res;

    quad(oper op, string arg11, string arg22, string resp){
        opp = op;
        arg1 = arg11;
        arg2 = arg22;
        res = resp;
    }
    


};

class TacGenerator{

    ASTNode* root;
    int temp_var_counter = 0;
    int line_counter = 0;
    std::list<quad> final;

    public:
    TacGenerator(ASTNode* rooot);

    std::list<quad> generate(ASTNode* node);

    string printTac();


};

#endif