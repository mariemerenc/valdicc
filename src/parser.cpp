#include "../headers/parser.h"
#include "../headers/usefultools.h"
#include <stdexcept>

using namespace std;

//retorna token atual
Token Parser::peek(){
    if(lookahead >= tokens.size()){
        return tokens.back();
    }
    return tokens[lookahead];
}

//sempre q chamamos o match() ele avança o lookahead :/ ent na hora de inserir na tabela de simbolos se o match fosse bem sucedido, ele nao teria acesso ao token atual, sempre o seguinte. esse previous() eh basicamente pra retornar o anterior pos match (ou seja, o "atual" ...?) 
Token Parser::previous(){
    if(lookahead == 0){
        return tokens[0];
    }
    return tokens[lookahead-1];
}

//metodo principal de erro
void Parser::throw_error(const std::string& msg){
    Token curr_token = peek();

    string full_msg = "Erro na linha " +
                    to_string(peek().line) +
                    ", coluna " +
                    to_string(peek().column) + 
                    ": " +
                    msg;
    
    throw runtime_error(full_msg);
}

//verificaçao do tipo do token recebido e mensagem q sera enviada caso os tipos nao batam
void Parser::match(TokenType expected_type, const string& custom_msg){
    if(peek().type == expected_type){
        lookahead++;
    }
    else{
        if(custom_msg.empty()){
            string msg = "ERRO GENERICO esperava " +
                        to_string(expected_type) +
                        " recebeu " +
                        to_string(peek().type);

            throw_error(msg);
        }
        else{
            throw_error(custom_msg);
        }
    }
}

//o metodo que vai começar a analise sintatica
void Parser::parse(){
    parse_Prog();

    if(peek().type != TokenType::END_OF_FILE){
        throw_error("MENSAGEM DE ERRO avisando q tem token dps do fim do arquivo");
    }
}

void Parser::parse_Prog(){
    parse_MainC();
    parse_DefCl();
}

void Parser::parse_MainC(){
    //class ABC { 
    match(TokenType::KW_CLASS);
    match(TokenType::IDENTIFIER, "CADE O NOME DA CLASSE PRINCIPAL");
    match(TokenType::PUNC_LBRACE);

    //public static void main(String[] args) {
    match(TokenType::KW_PUBLIC);
    match(TokenType::KW_STATIC);
    match(TokenType::KW_VOID, "main precisa ser do tipo void");
    match(TokenType::KW_MAIN, "cade o main");
    match(TokenType::PUNC_LPARENT);
    match(TokenType::KW_STRING, "o arg de main tem q ser string !");
    match(TokenType::PUNC_LBRACKET);
    match(TokenType::PUNC_RBRACKET);
    match(TokenType::IDENTIFIER, "cade o nome do argumento de main"); // id com o nome do argumento ne...
    match(TokenType::PUNC_RPARENT);
    match(TokenType::PUNC_LBRACE);
    
    parse_Cmd();

    //fim do main e fim da classe
    match(TokenType::PUNC_RBRACE, "faltou fechar o } de main");
    match(TokenType::PUNC_RBRACE, "faltou fechar o } da classe principal");
}

void Parser::parse_DefCl(){
    if(peek().type == TokenType::KW_CLASS){
        match(TokenType::KW_CLASS);
        match(TokenType::IDENTIFIER, "cade o nome da classe");
        parse_DefCl_prime();
    }
    //se nao começar com class eh lambda
}

void Parser::parse_DefCl_prime(){
    if(peek().type == TokenType::PUNC_LBRACE){
        match(TokenType::PUNC_LBRACE);
        parse_DefVar();
        parse_DefMet();
        match(TokenType::PUNC_RBRACE);
        parse_DefCl();
    }
    else if(peek().type == TokenType::KW_EXTENDS){
        match(TokenType::KW_EXTENDS);
        match(TokenType::IDENTIFIER, "cade o nome da classe dps de 'extends'");
        match(TokenType::PUNC_LBRACE);
        parse_DefVar();
        parse_DefMet();
        match(TokenType::PUNC_RBRACE);
        parse_DefCl();
    }
}