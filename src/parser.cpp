#include "../headers/parser.h"
#include "../headers/usefultools.h"
#include <stdexcept>
#include <limits.h>
using namespace std;

Parser::Parser(const std::vector<Token>& tokenss, RunningOptions opts) : tokens{tokenss}, m_running_opts{opts} {}


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


void Parser::throw_error(const std::string& msg, ErrorPhase phase){
    Token curr_token = peek();

    string label = (phase == ErrorPhase::SEMANTIC) ? "[ERRO SEMÂNTICO]" : "[ERRO SINTÁTICO]";

    string full_msg = label + " Na linha " +
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
    Token tkn_class = peek();
    match(TokenType::IDENTIFIER);
    env.insert(tkn_class.lexeme, "class", SymbolKind::CLASS, env.get_scope(), tkn_class.line, tkn_class.column);
    match(TokenType::PUNC_LBRACE);
    env.addTable("classe " + tkn_class.lexeme);

    //public static void main(String[] args) {
    match(TokenType::KW_PUBLIC);
    match(TokenType::KW_STATIC);
    match(TokenType::KW_VOID);
    match(TokenType::KW_MAIN);

    env.addTable("main");

    match(TokenType::PUNC_LPARENT);
    match(TokenType::KW_STRING);
    match(TokenType::PUNC_LBRACKET);
    match(TokenType::PUNC_RBRACKET);

    Token tkn_args = peek();
    match(TokenType::IDENTIFIER);

    env.insert(tkn_args.lexeme, "String[]", SymbolKind::VARIABLE, env.get_scope(), tkn_args.line, tkn_args.column);
    match(TokenType::PUNC_RPARENT);
    match(TokenType::PUNC_LBRACE);
    parse_Lcom();
    match(TokenType::PUNC_RBRACE);
    env.voltar();
    match(TokenType::PUNC_RBRACE);
    env.voltar();
}


void Parser::parse_DefCl(){
    while(peek().type == TokenType::KW_CLASS){
        match(TokenType::KW_CLASS);

        Token tkn_class = peek();
        match(TokenType::IDENTIFIER);

        //std::string parent = "";
        if(peek().type == TokenType::KW_EXTENDS){
            match(TokenType::KW_EXTENDS);
            //parent = peek().lexeme;
            match(TokenType::IDENTIFIER);
        }

        bool success = env.insert(tkn_class.lexeme, "class", SymbolKind::CLASS, env.get_scope(), tkn_class.line, tkn_class.column);

        if(!success){
            throw_error("Classe já declarada: " + tkn_class.lexeme, ErrorPhase::SEMANTIC);
        }

        match(TokenType::PUNC_LBRACE);
        env.addTable("classe " + tkn_class.lexeme);
        parse_DefVar();
        parse_DefMet();
        match(TokenType::PUNC_RBRACE);
        env.voltar();
    }
}


void Parser::parse_DefVar() {
    // Uses lookahead (peek_next) to resolve structural ambiguity.
    while ( peek().type == TokenType::KW_INT ||
            peek().type == TokenType::KW_BOOLEAN ||
            (peek().type == TokenType::IDENTIFIER && peek_next().type == TokenType::IDENTIFIER)){
        std::string tkn_type = parse_Type_str();
        Token tkn_id = peek();
        match(TokenType::IDENTIFIER);

        bool success = env.insert(tkn_id.lexeme, tkn_type, SymbolKind::VARIABLE, env.get_scope(), tkn_id.line, tkn_id.column);

        if(!success){
            throw_error("Variável já declarada neste escopo: " + tkn_id.lexeme, ErrorPhase::SEMANTIC);
        }
        match(TokenType::PUNC_SEMICOLON);
    }
}


void Parser::parse_DefMet() {
    while(peek().type == TokenType::KW_PUBLIC){
        match(TokenType::KW_PUBLIC);
        
        std::string tkn_return_type = parse_Type_str(); 

        Token tkn_met = peek();
        match(TokenType::IDENTIFIER);

        bool success = env.insert(tkn_met.lexeme, tkn_return_type, SymbolKind::METHOD, env.get_scope(), tkn_met.line, tkn_met.column);

        if(!success){
            throw_error("Método já declarado: " + tkn_met.lexeme, ErrorPhase::SEMANTIC);
        }

        env.addTable("método " + tkn_met.lexeme);  // criar tabela antes de ler os args para as variaveis de parse_Args e parse_DefVar ficarem jutnas

        match(TokenType::PUNC_LPARENT);
        if(peek().type != TokenType::PUNC_RPARENT){
            parse_Args();
        }
        match(TokenType::PUNC_RPARENT);
        match(TokenType::PUNC_LBRACE);
        parse_DefVar();
        
        while ( peek().type == TokenType::IDENTIFIER ||
                peek().type == TokenType::KW_IF ||
                peek().type == TokenType::KW_WHILE ||
                peek().type == TokenType::KW_SYSTEM || 
                peek().type == TokenType::PUNC_LBRACE){
                parse_Cmd();
            }

        match(TokenType::KW_RETURN);
        parse_Exp();
        match(TokenType::PUNC_SEMICOLON);
        match(TokenType::PUNC_RBRACE);
        env.voltar();
    }
}

std::string Parser::parse_Type_str() {
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
        return "boolean";
    }

    else {
        std::string name = peek().lexeme;
        match(TokenType::IDENTIFIER);
        return name;
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
    std::string tkn_type = parse_Type_str();
    Token tkn_id = peek();
    match(TokenType::IDENTIFIER);

    //isso aq vai dar errado ja q podemos passar uma classe como argumento........ e por enquanto so tamo deixando variavel
    bool success = env.insert(tkn_id.lexeme, tkn_type, SymbolKind::VARIABLE, env.get_scope(), tkn_id.line, tkn_id.column);

    if(!success){
        throw_error("Tentativa de inserção de parâmetro duplicado: " + tkn_id.lexeme, ErrorPhase::SEMANTIC);
    }
    
    while(peek().type == TokenType::PUNC_COMMA){
        match(TokenType::PUNC_COMMA);
        tkn_type = parse_Type_str();
        tkn_id = peek();
        match(TokenType::IDENTIFIER);

        //isso aq vai dar errado pt.2 ja q podemos passar uma classe como argumento........ e por enquanto so tamo deixando variavel
        success = env.insert(tkn_id.lexeme, tkn_type, SymbolKind::VARIABLE, env.get_scope(), tkn_id.line, tkn_id.column);

        if(!success){
            throw_error("Tentativa de inserção de parâmetro duplicado: " + tkn_id.lexeme, ErrorPhase::SEMANTIC);
        }
    }
}

void Parser::parse_Lcom(){
    parse_Cmd();

    // { agindo como um iniciador de comando para resolver bugs de if { exp } ou if exp 
    while ( peek().type == TokenType::IDENTIFIER ||
            peek().type == TokenType::KW_IF ||
            peek().type == TokenType::KW_WHILE ||
            peek().type == TokenType::KW_SYSTEM || 
            peek().type == TokenType::PUNC_LBRACE){
                parse_Cmd();
            }
}

void Parser::parse_Cmd() {
    if(peek().type == TokenType::PUNC_LBRACE){
        match(TokenType::PUNC_LBRACE);
        env.addTable("bloco");

        if(peek().type != TokenType::PUNC_RBRACE){
            parse_Lcom();
        }
        match(TokenType::PUNC_RBRACE);
        env.voltar();
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
        Token tkn_id = peek();
        match(TokenType::IDENTIFIER);
        
        Symbol* symb = env.lookup(tkn_id.lexeme);
        if(symb == nullptr){
            throw_error("Variável não declarada neste escopo: " + tkn_id.lexeme, ErrorPhase::SEMANTIC);
        }

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

void Parser::parse_Exp() {
    parse_And_exp();
}

void Parser::parse_And_exp(){
    parse_Rel_exp();

    while(peek().type == TokenType::OP_AND){
        match(TokenType::OP_AND);
        parse_Rel_exp();
    }
}

void Parser::parse_Rel_exp(){
    parse_Add_exp();

    while(peek().type == TokenType::OP_GREATER){
        match(TokenType::OP_GREATER);
        parse_Add_exp();
    }
}

void Parser::parse_Add_exp(){
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

void Parser::parse_Mul_exp(){
    parse_Un_exp();

    while(peek().type == TokenType::OP_ASTERISK){
        match(TokenType::OP_ASTERISK);
        parse_Un_exp();
    }
}

void Parser::parse_Un_exp(){
    if(peek().type == TokenType::OP_NOT){
        match(TokenType::OP_NOT);
        parse_Un_exp();
    }
    else{
        parse_Psf_exp();
    }
}

void Parser::parse_Psf_exp(){
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

void Parser::parse_Pri_exp(){
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
        Token tkn_id = peek();
        match(TokenType::IDENTIFIER);

        Symbol* symb = env.lookup(tkn_id.lexeme);
        if(symb == nullptr){
            throw_error("Variável não declarada neste escopo: " + tkn_id.lexeme, ErrorPhase::SEMANTIC);
        }

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
            Token tkn_class = peek();
            match(TokenType::IDENTIFIER);
            match(TokenType::PUNC_LPARENT);
            match(TokenType::PUNC_RPARENT);

            Symbol* symb = env.lookup(tkn_class.lexeme);
            if(symb == nullptr || (*symb).kind != SymbolKind::CLASS){
                throw_error("Classe não declarada: " + tkn_class.lexeme, ErrorPhase::SEMANTIC);
            }
        }
    }
    else{
        throw_error("Esperava o começo de uma Exp.");
    }
}

void Parser::parse_ListExp() {
    parse_Exp();
    while(peek().type == TokenType::PUNC_COMMA){
        match(TokenType::PUNC_COMMA);
        parse_Exp();
    }
}