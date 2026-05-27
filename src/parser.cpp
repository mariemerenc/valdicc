#include "../headers/parser.h"
#include "../headers/usefultools.h"
#include <stdexcept>
#include <limits.h>
using namespace std;

Parser::Parser(const std::vector<Token>& tokenss, SymbolTable& symbol_tablee) : tokens{tokenss}, symbol_table{symbol_tablee} { }


Token Parser::peek(){
    if(lookahead >= tokens.size()){
        return tokens.back();
    }
    return tokens[lookahead];
}


Token Parser::peek_next(){
    if(lookahead+1 >= tokens.size()){
        return tokens.back();
    }
    return tokens[lookahead+1];
}


Token Parser::previous(){
    if(lookahead == 0){
        return tokens[0];
    }
    return tokens[lookahead-1];
}


void Parser::throw_error(const std::string& msg){
    Token curr_token = peek();

    string full_msg = "Erro na linha " +
                    to_string(curr_token.line) +
                    ", coluna " +
                    to_string(curr_token.column) + 
                    ": " +
                    msg;
    

    if(curr_token.type == TokenType::IDENTIFIER){
        string best_match;
        int best_dist = INT_MAX;

        for(const auto& [keyword, _] : KeywordMap){
            int dist = UFT::levenshtein_distance(curr_token.lexeme, keyword);
            if(dist < best_dist){
                best_dist = dist;
                best_match = keyword;
            }
        }

        if(best_dist <= 2){
            full_msg += ". Você quis dizer: '" + best_match + "'?";
        }
    }
    throw runtime_error(full_msg);
}


void Parser::match(TokenType expected_type, const string& custom_msg){
    if(peek().type == expected_type){
        lookahead++;
    }
    else{
        if(custom_msg.empty()){
            string msg = "Esperava " +
                        to_string(expected_type) +
                        ", recebeu " +
                        to_string(peek().type);

            throw_error(msg);
        }
        else{
            throw_error(custom_msg);
        }
    }
}


void Parser::parse(){
    parse_Prog();

    if(lookahead < tokens.size() && peek().type != TokenType::END_OF_FILE){
        throw_error("Presença de tokens após EOF.");
    }
}


void Parser::parse_Prog(){
    parse_MainC();
    parse_DefCl();
}


void Parser::parse_MainC(){
    //class ABC { 
    match(TokenType::KW_CLASS);
    match(TokenType::IDENTIFIER);
    match(TokenType::PUNC_LBRACE);

    //public static void main(String[] args) {
    match(TokenType::KW_PUBLIC);
    match(TokenType::KW_STATIC);
    match(TokenType::KW_VOID);
    match(TokenType::KW_MAIN);
    match(TokenType::PUNC_LPARENT);
    match(TokenType::KW_STRING);
    match(TokenType::PUNC_LBRACKET);
    match(TokenType::PUNC_RBRACKET);
    match(TokenType::IDENTIFIER);
    match(TokenType::PUNC_RPARENT);
    match(TokenType::PUNC_LBRACE);
    parse_Cmd();
    match(TokenType::PUNC_RBRACE);
    match(TokenType::PUNC_RBRACE);
}


void Parser::parse_DefCl(){
    while(peek().type == TokenType::KW_CLASS){
        match(TokenType::KW_CLASS);
        match(TokenType::IDENTIFIER);

        if(peek().type == TokenType::KW_EXTENDS){
            match(TokenType::KW_EXTENDS);
            match(TokenType::IDENTIFIER);
        }

        match(TokenType::PUNC_LBRACE);
        parse_DefVar();
        parse_DefMet();
        match(TokenType::PUNC_RBRACE);
    }
}


void Parser::parse_DefVar() {
    // Uses lookahead (peek_next) to resolve structural ambiguity.
    while ( peek().type == TokenType::KW_INT ||
            peek().type == TokenType::KW_BOOLEAN ||
            (peek().type == TokenType::IDENTIFIER && peek_next().type == TokenType::IDENTIFIER)){
        parse_Type();
        match(TokenType::IDENTIFIER);
        match(TokenType::PUNC_SEMICOLON);
    }
}


void Parser::parse_DefMet() {
    while(peek().type == TokenType::KW_PUBLIC){
        match(TokenType::KW_PUBLIC);
        parse_Type();
        match(TokenType::IDENTIFIER);
        match(TokenType::PUNC_LPARENT);
        if(peek().type != TokenType::PUNC_RPARENT){
            parse_Args();
        }
        match(TokenType::PUNC_RPARENT);
        match(TokenType::PUNC_LBRACE);
        parse_DefVar();
        parse_Cmd();
        match(TokenType::KW_RETURN);
        parse_Exp_og();
        match(TokenType::PUNC_SEMICOLON);
        match(TokenType::PUNC_RBRACE);
    }
}


void Parser::parse_Type() {
    if (peek().type == TokenType::KW_INT) {
        match(TokenType::KW_INT);

        if (peek().type == TokenType::PUNC_LBRACKET) {
            match(TokenType::PUNC_LBRACKET);
            match(TokenType::PUNC_RBRACKET);
        }
    }

    else if (peek().type == TokenType::KW_BOOLEAN) {
        match(TokenType::KW_BOOLEAN);
    }

    else {
        match(TokenType::IDENTIFIER);
    }
}


void Parser::parse_Args() {
    parse_Type();
    match(TokenType::IDENTIFIER);
    
    while(peek().type == TokenType::PUNC_COMMA){
        match(TokenType::PUNC_COMMA);
        parse_Type();
        match(TokenType::IDENTIFIER);
    }
}


void Parser::parse_Cmd() {
    if (peek().type == TokenType::PUNC_LBRACE) {
        match(TokenType::PUNC_LBRACE);
        parse_Cmd();
        match(TokenType::PUNC_RBRACE);
    }
    else if (peek().type == TokenType::KW_IF) {
        match(TokenType::KW_IF);
        match(TokenType::PUNC_LPARENT);
        parse_Exp_og();
        match(TokenType::PUNC_RPARENT);
        parse_Cmd();
        match(TokenType::KW_ELSE);
        parse_Cmd();
    }
    else if (peek().type == TokenType::KW_WHILE) {
        match(TokenType::KW_WHILE);
        match(TokenType::PUNC_LPARENT);
        parse_Exp_og();
        match(TokenType::PUNC_RPARENT);
        parse_Cmd();
    }
    else if (peek().type == TokenType::KW_SYSTEM) {
        match(TokenType::KW_SYSTEM);
        match(TokenType::PUNC_DOT);
        match(TokenType::KW_OUT);
        match(TokenType::PUNC_DOT);
        match(TokenType::KW_PRINTLN);
        match(TokenType::PUNC_LPARENT);
        parse_Exp_og();
        match(TokenType::PUNC_RPARENT);
        match(TokenType::PUNC_SEMICOLON);
    }
    // left factoring applied here
    else {
        match(TokenType::IDENTIFIER);
        
        if (peek().type == TokenType::PUNC_LBRACKET) {
            match(TokenType::PUNC_LBRACKET);
            parse_Exp_og();
            match(TokenType::PUNC_RBRACKET);
        }

        match(TokenType::OP_ASSIGN);
        parse_Exp_og();
        match(TokenType::PUNC_SEMICOLON);
    }
}


bool Parser::is_exp_tail(TokenType type){
    switch(type){
        case TokenType::OP_AND:
        case TokenType::OP_GREATER:
        case TokenType::OP_PLUS:
        case TokenType::OP_MINUS:
        case TokenType::OP_ASTERISK:
        case TokenType::PUNC_LBRACKET:
        case TokenType::PUNC_DOT:
            return true;
        
        default:
            return false;
    }
}


void Parser::parse_Exp_og(){
    if(peek().type == TokenType::OP_NOT){
        match(TokenType::OP_NOT);
        parse_Exp_og();
    }
    else if(peek().type == TokenType::PUNC_LPARENT){
        match(TokenType::PUNC_LPARENT);
        parse_Exp_og();
        match(TokenType::PUNC_RPARENT);
    }
    else if(peek().type == TokenType::KW_TRUE){
        match(TokenType::KW_TRUE);
    }
    else if(peek().type == TokenType::KW_FALSE){
        match(TokenType::KW_FALSE);
    }
    else if(peek().type == TokenType::IDENTIFIER){
        match(TokenType::IDENTIFIER);
    }
    else if(peek().type == TokenType::NUMBER_LITERAL){
        match(TokenType::NUMBER_LITERAL);
    }
    else if(peek().type == TokenType::KW_THIS){
        match(TokenType::KW_THIS);
    }
    else if(peek().type == TokenType::KW_NEW){
        match(TokenType::KW_NEW);
        if(peek().type == TokenType::KW_INT){
            match(TokenType::KW_INT);
            match(TokenType::PUNC_LBRACKET);
            parse_Exp_og();
            match(TokenType::PUNC_RBRACKET);
        }
        else{
            match(TokenType::IDENTIFIER);
            match(TokenType::PUNC_LPARENT);
            match(TokenType::PUNC_RPARENT);
        }
    }
    else{
        throw_error("Esperava o começo de uma Exp.");
    }

    // this loop eliminates left recursion by parsing the base expressions 
    // first, then consuming the operators and RHS expressions 
    while(is_exp_tail(peek().type)){
        switch(peek().type){
            case TokenType::OP_AND:
                match(TokenType::OP_AND);
                parse_Exp_og();
                break;

            case TokenType::OP_GREATER:
                match(TokenType::OP_GREATER);
                parse_Exp_og();
                break;

            case TokenType::OP_PLUS:
                match(TokenType::OP_PLUS);
                parse_Exp_og();
                break;
            
            case TokenType::OP_MINUS:
                match(TokenType::OP_MINUS);
                parse_Exp_og();
                break;
            
            case TokenType::OP_ASTERISK:
                match(TokenType::OP_ASTERISK);
                parse_Exp_og();
                break;
            
            case TokenType::PUNC_LBRACKET:
                match(TokenType::PUNC_LBRACKET);
                parse_Exp_og();
                match(TokenType::PUNC_RBRACKET);
                break;
            
            case TokenType::PUNC_DOT:
                //left factoring applied here
                match(TokenType::PUNC_DOT);
                if(peek().type == TokenType::KW_LENGTH){
                    match(TokenType::KW_LENGTH);
                }
                else{
                    match(TokenType::IDENTIFIER);
                    match(TokenType::PUNC_LPARENT);
                    //handling zero argument function
                    if(peek().type != TokenType::PUNC_RPARENT){
                        parse_ListExp(); 
                    }
                    match(TokenType::PUNC_RPARENT);
                }
                break;
            
            default:
                throw_error("Operador inválido!");
                break;
        }
    }

}


void Parser::parse_ListExp() {
    parse_Exp_og();
    while(peek().type == TokenType::PUNC_COMMA){
        match(TokenType::PUNC_COMMA);
        parse_Exp_og();
    }
}


/*--------------------------------------------------[AQUI COMEÇA A PRECEDÊNCIA DE OPERADORES]--------------------------------------------------*/

void Parser::parse_Exp() {
    parse_AssignExp();
}

void Parser::parse_AssignExp(){

}

void Parser::parse_BoolExp() {
    parse_ComparisonExp();

    if (peek().type == TokenType::OP_AND) {
        match(TokenType::OP_AND);
    }

    // se nao, eh so o comparison
}



void Parser::parse_ComparisonExp(){
    parse_AddExp();
    
    if (peek().type == TokenType::OP_GREATER) {
        match(TokenType::OP_GREATER);
        parse_ComparisonExp();
    }

    // else eh lambda
}

void Parser::parse_AddExp(){
    parse_MulExp();

    if (peek().type == TokenType::OP_PLUS) {
        //
    }
}

void Parser::parse_MulExp(){

}

void Parser::parse_PrimaryExpression(){

}

void Parser::parse_IndexExp(){

}

void Parser::parse_ArrayInit(){

}

void Parser::parse_LiteralList(){

}

void Parser::parse_Literal(){ // idk what this is, eh um lexema of some sort?

}

void Parser::parse_MemberExp(){

}

void Parser::parse_FuncCall(){

}

void Parser::parse_ExpList(){

}



