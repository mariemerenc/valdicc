#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <map>
#include <string>
#include "token.h"
#include "symbol_table.h"

class Parser{
    public:
    Parser(const std::vector<Token>& tokenss, SymbolTable& symbol_tablee); 

    void parse(); // começa pela regra Prog. eh tipo o pontape inicial

    private:
    std::vector<Token> tokens; // a lista de tokens recebida do lexer
    SymbolTable& symbol_table; // a tabela de simbolos
    size_t lookahead = 0; // o indice q aponta para qual token estamos olhando

    Token peek(); // uma olhada pro token seguinte sem avançar o indice
    Token peek_next(); //uma olhada pra 2 tokens da frente sem avançar o indice
    Token previous();
    void match(TokenType expected_type, const std::string& custom_msg = ""); // verificaçao do tipo de token esperado
    void throw_error(const std::string& msg); // a funçao principal de erro ! as mensagens variam

    void parse_Prog();

    void parse_MainC();

    void parse_DefCl();
    void parse_DefCl_prime();

    void parse_DefVar();

    void parse_DefMet();
    void parse_DefMet_prime();

    void parse_Type();
    void parse_Type_prime();

    void parse_Args();
    void parse_Args_prime();
    
    void parse_Cmd();
    void parse_Cmd_prime();

    bool is_exp_tail(TokenType type);
    void parse_Exp_og(); // parsa sem PEMDAS par caso não conseguimos implementar por completo a tempo

    void parse_Exp();
    void parse_AssignExp();
    void parse_BoolExp();
    void parse_ComparisonExp();
    void parse_AddExp();
    void parse_MulExp();
    void parse_PrimaryExpression();
    void parse_IndexExp();
    void parse_ArrayInit();
    void parse_LiteralList();
    void parse_Literal(); // idk what this is, eh um lexema of some sort?
    void parse_MemberExp();
    void parse_FuncCall();
    void parse_ExpList();
    
    //void parse_Exp_prime();
    //void parse_Dot_prime();

    void parse_ListExp();
    void parse_ListExp_prime();

};

#endif