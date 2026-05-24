#include "../headers/parser.h"
#include "../headers/usefultools.h"
#include <stdexcept>

// NOTAS: LEMBRAR DE
// SEMPRE USAR O ELSE IF, SE NAO NAO EH EXCLUSIVO E FODE COM O PARSER
// PARENTHESIS = ()
// BRACKETS = []
// BRACES = {}

using namespace std;

//inicializa o parser

Parser::Parser(const std::vector<Token>& tokenss, SymbolTable& symbol_tablee) : tokens{tokenss}, symbol_table{symbol_tablee} {
}


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

void Parser::parse_DefVar() {
    if (peek().type == TokenType::KW_INT) {
        match(TokenType::KW_INT);
        if (peek().type == TokenType::PUNC_LBRACE) { // eh um array kinda; preciso fazer outra produção pra isso sera?
            match(TokenType::PUNC_LBRACE);
            match(TokenType::PUNC_RBRACE);
        }
        // aq eu matcho o id mas eh um lexema ja
        match(TokenType::IDENTIFIER);
        match(TokenType::PUNC_SEMICOLON);
    }

    else if (peek().type == TokenType::KW_BOOLEAN) {
        match(TokenType::KW_BOOLEAN);
    }

    else if (peek().type == TokenType::IDENTIFIER) { // SO COLOQUEI PRA NAO DAR ERRO, PRECISO REVER THIS
        match(TokenType::IDENTIFIER);
    }

    else {
        // lambda production!
        return;
    }

    // so mt genial papo reto
    // so retorna se nao matchou nenhum dos firsts entao se chegou ate aq eh porq produziu algo
    // PARSES ID
    // BY THAT I DO MEAN RECOGNIZES LEXEME
    // note: isso me parece fake....... em que mundo podemos ter identifier depois de identifier?

    match(TokenType::IDENTIFIER);
    match(TokenType::PUNC_SEMICOLON);
    parse_DefVar();
}

void Parser::parse_DefMet() {
    if (peek().type == TokenType::KW_PUBLIC) {
        match(TokenType::KW_PUBLIC);
        parse_Type();
        match(TokenType::IDENTIFIER);
        match(TokenType::PUNC_LPARENT);
        parse_DefMet_prime();

    }

    // else eh uma producao lambda
}

void Parser::parse_DefMet_prime() {

    if (peek().type != TokenType::PUNC_RPARENT) { // tem algo antes do ) -> argumentos!
        parse_Args();
    }

    match(TokenType::PUNC_RPARENT);
    match(TokenType::PUNC_LBRACE);
    parse_DefVar();
    parse_Cmd();
    match(TokenType::KW_RETURN);
    parse_Exp();
    match(TokenType::PUNC_SEMICOLON);
    match(TokenType::PUNC_RBRACE);
    parse_DefMet();
}

void Parser::parse_Type() {

    if (peek().type == TokenType::KW_INT) {
        match(TokenType::KW_INT);
        parse_Type_prime();
    }

    else if (peek().type == TokenType::KW_BOOLEAN) {
        match(TokenType::KW_BOOLEAN);
    }

    else {
        match(TokenType::IDENTIFIER);
    }
}

void Parser::parse_Type_prime() {

    if (peek().type == TokenType::PUNC_RBRACKET) {
        match(TokenType::PUNC_RBRACKET);
        match(TokenType::PUNC_LBRACKET);
    }
    
    // else eh lambda
}


void Parser::parse_Args() {
    parse_Type();
    // o parse id eh equivalente a identificar um lexema / identifier
    match(TokenType::IDENTIFIER);
    parse_Args_prime();
}

void Parser::parse_Args_prime(){
    if (peek().type == TokenType::PUNC_COMMA) {
        match(TokenType::PUNC_COMMA);
        parse_Args();
    }
    // lambda production
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
        parse_Exp();
        match(TokenType::PUNC_RPARENT);
        parse_Cmd();
        match(TokenType::KW_ELSE);
        parse_Cmd();
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
    else { // precisa ser um id
        match(TokenType::IDENTIFIER);
        parse_Cmd_prime();
    }
}

void Parser::parse_Cmd_prime() {

    if (peek().type == TokenType::OP_ASSIGN) {
        match(TokenType::OP_ASSIGN);
    }

    else {
        match(TokenType::PUNC_LBRACKET);
        parse_Exp();
        match(TokenType::PUNC_RBRACKET);
    }

    parse_Exp();
    match(TokenType::PUNC_SEMICOLON);

}

// implementação original sem precedência de operadores!
/*void Parser::parse_Exp_og() {
    // skeete vai fazer a parte de precedencia de operadores
}

void Parser::parse_ListExp() {
    if (false /*FAZER ISSO AQQQ) { // depende de exp!

    }

    // else eh lambda
}


void Parser::parse_ListExp_prime() {
    if (peek().type == TokenType::PUNC_COMMA) {
        match(TokenType::PUNC_COMMA);
        parse_ListExp();
    }

    // aq eh lambda
}*/

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



