#include "../headers/parser.h"
#include "../headers/usefultools.h"
#include <memory>
#include <stdexcept>
#include <limits.h>
#include <vector>
using namespace std;

Parser::Parser(const std::vector<Token>& tokenss, SymbolTable& symbol_tablee, RunningOptions opts) : tokens{tokenss}, symbol_table{symbol_tablee}, m_running_opts{opts} { }
using NodePtr = std::unique_ptr<ASTNode>;
using NodeVec = std::vector<NodePtr>;
using ExprNodePtr = std::unique_ptr<ExprNode>;

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
    

    if(curr_token.type == TokenType::IDENTIFIER && m_running_opts.suggest_corrections){
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


AST Parser::parse(){
    
    AST tree(parse_Prog());

    if(lookahead < tokens.size() && peek().type != TokenType::END_OF_FILE){
        throw_error("Presença de tokens após EOF.");
    }
    return tree;
}


NodePtr Parser::parse_Prog(){
    auto m = parse_MainC();
    auto d = parse_DefCl();
    auto p = make_unique<node_types::ProgNode>(std::move(m), std::move(d));
    return p;
}


NodePtr Parser::parse_MainC(){
    //class ABC { 
    match(TokenType::KW_CLASS);
    match(TokenType::IDENTIFIER);
    string main_class_id = previous().lexeme;
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
    string args_id = previous().lexeme;
    match(TokenType::PUNC_RPARENT);
    match(TokenType::PUNC_LBRACE);
    auto command_list = parse_Lcom();
    match(TokenType::PUNC_RBRACE);
    match(TokenType::PUNC_RBRACE);
    return std::make_unique<node_types::MainDecl>(main_class_id, args_id, std::move(command_list));
}


NodeVec Parser::parse_DefCl(){
    NodeVec class_definitions;
    while(peek().type == TokenType::KW_CLASS){
        match(TokenType::KW_CLASS);
        match(TokenType::IDENTIFIER);
        string class_id = previous().lexeme;
        bool extends = false;
        string extends_id = "";
        if(peek().type == TokenType::KW_EXTENDS){
            match(TokenType::KW_EXTENDS);
            match(TokenType::IDENTIFIER);
            extends = true;
            extends_id = previous().lexeme;
        }
        match(TokenType::PUNC_LBRACE);
        NodeVec vars(parse_DefVar());
        NodeVec methods(parse_DefMet());
        match(TokenType::PUNC_RBRACE);
        class_definitions.push_back(std::make_unique<node_types::ClassDecl>(class_id, std::move(vars),
                                     std::move(methods), extends, extends_id));
    }
    return class_definitions;
}


NodeVec Parser::parse_DefVar() {
    // Uses lookahead (peek_next) to resolve structural ambiguity.
    NodeVec vars;
    while ( peek().type == TokenType::KW_INT ||
            peek().type == TokenType::KW_BOOLEAN ||
            (peek().type == TokenType::IDENTIFIER && peek_next().type == TokenType::IDENTIFIER)){
        string type = parse_Type();
        match(TokenType::IDENTIFIER);
        string id = previous().lexeme;
        match(TokenType::PUNC_SEMICOLON);
        vars.push_back(std::make_unique<node_types::VarDecl>(type, id));
    }
    return vars;
}


NodeVec Parser::parse_DefMet() {
    NodeVec methods;
    while(peek().type == TokenType::KW_PUBLIC){
        match(TokenType::KW_PUBLIC);
        auto type = parse_Type();
        match(TokenType::IDENTIFIER);
        string id = previous().lexeme;
        match(TokenType::PUNC_LPARENT);
        NodeVec args;
        if(peek().type != TokenType::PUNC_RPARENT){
            args = parse_Args();
        }
        match(TokenType::PUNC_RPARENT);
        match(TokenType::PUNC_LBRACE);
        NodeVec vars = parse_DefVar();
        NodeVec commands;
        while ( peek().type == TokenType::IDENTIFIER ||
                peek().type == TokenType::KW_IF ||
                peek().type == TokenType::KW_WHILE ||
                peek().type == TokenType::KW_SYSTEM || 
                peek().type == TokenType::PUNC_LBRACE){
                commands.push_back(parse_Cmd());
            }
        match(TokenType::KW_RETURN);
        ExprNodePtr expr = parse_Exp();
        match(TokenType::PUNC_SEMICOLON);
        match(TokenType::PUNC_RBRACE);
        methods.push_back(std::make_unique<node_types::MethodDecl>(id, type, std::move(args), std::move(commands), std::move(expr)));
    }
    return methods;
}


string Parser::parse_Type() {
    if (peek().type == TokenType::KW_INT) {
        match(TokenType::KW_INT);
        if (peek().type == TokenType::PUNC_LBRACKET) {
            match(TokenType::PUNC_LBRACKET);
            match(TokenType::PUNC_RBRACKET);
            return "int[]";
        }
        return "int";
    }

    else if (peek().type == TokenType::KW_BOOLEAN) {
        match(TokenType::KW_BOOLEAN);
        return "bool";
    }

    else {
        match(TokenType::IDENTIFIER);
        return previous().lexeme;
    }
}


NodeVec Parser::parse_Args() {
    NodeVec args;
    auto first_type = parse_Type();
    match(TokenType::IDENTIFIER);
    auto first_id = previous().lexeme;
    while(peek().type == TokenType::PUNC_COMMA){
        match(TokenType::PUNC_COMMA);
        parse_Type();
        match(TokenType::IDENTIFIER);
    }
}

NodeVec Parser::parse_Lcom(){
    NodeVec commands;
    commands.push_back(parse_Cmd());

    // { agindo como um iniciador de comando para resolver bugs de if { exp } ou if exp 
    while ( peek().type == TokenType::IDENTIFIER ||
            peek().type == TokenType::KW_IF ||
            peek().type == TokenType::KW_WHILE ||
            peek().type == TokenType::KW_SYSTEM || 
            peek().type == TokenType::PUNC_LBRACE){
        commands.push_back(parse_Cmd());
    }
    return commands;
}

NodePtr Parser::parse_Cmd() {
    if(peek().type == TokenType::PUNC_LBRACE){
        match(TokenType::PUNC_LBRACE);

        if(peek().type != TokenType::PUNC_RBRACE){
            parse_Lcom();
        }
        match(TokenType::PUNC_RBRACE);
    }
    else if (peek().type == TokenType::KW_IF) {
        match(TokenType::KW_IF);
        match(TokenType::PUNC_LPARENT);
        parse_Exp();
        match(TokenType::PUNC_RPARENT);
        parse_Cmd();

        //I --> else { Lcom } | lambda
        if(peek().type == TokenType::KW_ELSE){
            match(TokenType::KW_ELSE);
            parse_Cmd();
        }
    }
    else if (peek().type == TokenType::KW_WHILE) {
        match(TokenType::KW_WHILE);
        match(TokenType::PUNC_LPARENT);
        parse_Exp();
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
        parse_Exp();
        match(TokenType::PUNC_RPARENT);
        match(TokenType::PUNC_SEMICOLON);
    }
    // left factoring applied here
    else {
        match(TokenType::IDENTIFIER);
        
        if (peek().type == TokenType::PUNC_LBRACKET) {
            match(TokenType::PUNC_LBRACKET);
            parse_Exp();
            match(TokenType::PUNC_RBRACKET);
        }

        match(TokenType::OP_ASSIGN);
        parse_Exp();
        match(TokenType::PUNC_SEMICOLON);
    }
}

ExprNodePtr Parser::parse_Exp() {
    parse_And_exp();
}

ExprNodePtr Parser::parse_And_exp(){
    parse_Rel_exp();

    while(peek().type == TokenType::OP_AND){
        match(TokenType::OP_AND);
        parse_Rel_exp();
    }
}

ExprNodePtr Parser::parse_Rel_exp(){
    parse_Add_exp();

    while(peek().type == TokenType::OP_GREATER){
        match(TokenType::OP_GREATER);
        parse_Add_exp();
    }
}

ExprNodePtr Parser::parse_Add_exp(){
    parse_Mul_exp();

    while(peek().type == TokenType::OP_PLUS || peek().type == TokenType::OP_MINUS){
        if(peek().type == TokenType::OP_PLUS){
            match(TokenType::OP_PLUS);
        }
        else{
            match(TokenType::OP_MINUS);
        }
        parse_Mul_exp();
    }
}

ExprNodePtr Parser::parse_Mul_exp(){
    parse_Un_exp();

    while(peek().type == TokenType::OP_ASTERISK){
        match(TokenType::OP_ASTERISK);
        parse_Un_exp();
    }
}

ExprNodePtr Parser::parse_Un_exp(){
    if(peek().type == TokenType::OP_NOT){
        match(TokenType::OP_NOT);
        parse_Un_exp();
    }
    else{
        parse_Psf_exp();
    }
}

ExprNodePtr Parser::parse_Psf_exp(){
    parse_Pri_exp();

    while(peek().type == TokenType::PUNC_LBRACKET || peek().type == TokenType::PUNC_DOT){
        if(peek().type == TokenType::PUNC_LBRACKET){
            match(TokenType::PUNC_LBRACKET);
            parse_Exp();
            match(TokenType::PUNC_RBRACKET);
        }
        else{
            match(TokenType::PUNC_DOT);
            //left factoring applied here
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
        }
        
    }
}

ExprNodePtr Parser::parse_Pri_exp(){
    if(peek().type == TokenType::PUNC_LPARENT){
        match(TokenType::PUNC_LPARENT);
        parse_Exp();
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
            parse_Exp();
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
}

vector<ExprNodePtr> Parser::parse_ListExp() {
    vector<ExprNodePtr> ret_vec;
    ret_vec.push_back(parse_Exp());
    while(peek().type == TokenType::PUNC_COMMA){
        match(TokenType::PUNC_COMMA);
        ret_vec.push_back(parse_Exp());
    }
    return ret_vec;
}