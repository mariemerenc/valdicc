#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include <unordered_map>
#include "AST.h"

class TypeChecker{
    ASTNode* root;

    public:
    TypeChecker(ASTNode* root);
    void check(); // metodo principal da checagem com duas passagens pela AST

    // [GLOBAIS] o que vai ser povoado na primeira passagem 
    std::unordered_map<string, string> class_parent; // classe | superclasse (p/ guardar herança)
    std::unordered_map<string, vector<string>>  method_params; // metodo | tipos dos params
    std::unordered_map<string, string> method_return; // metodo | tipo do retorno
    std::unordered_map<string, std::unordered_map<string, string>>  class_attributes; //classe | [atributo | tipo]

    // [ATUAIS/contextuais?] informaçoes referentes a "localizaçao" ou contexto na AST
    string current_class; //nome da classe sendo checada
    std::unordered_map<string, string> current_method_vars; // nome | tipo

    private:
    void collect(); // primeira passagem pela AST para registrar todas as declaraçoes

    string check_type_of(ExprNode* expr); // 4.1 semantica dos tipos
    void check_class(node_types::ClassDecl* cl); // 4.2 semantica das classes 
    std::string check_variable(std::string id, ASTNode* location); // 4.3 semantica das variaveis 
    void check_method(node_types::MethodDecl* met); // 4.6.1 semantica da chamada de metodos (tipo do retorno) e 4.3.1 semantica das variaveis - parametros do metodo ... ? nao sei na real
    void check_command(ASTNode* cmd); // 4.4 semantica dos comandos

    void throw_semantic_error(ASTNode* node, const std::string& msg);

};

#endif



/*
notas de mariana

MINHA IDEIA PARA O CHECADOR DE TIPOS
a primeira passagem pela AST (collect()) eh responsavel pelo registro das declaraçoes nos mapas globais (das heranças das classes, dos parametros dos metodos, dos retornos dos metodos, (dos tipos) dos atributos das classes) antes da checagem propriamente dita 

a segunda passagem vai percorrer o main e dps cada classe verificando os tipos e usando os mapas povoados pela primeira passagem (checagem independente da ordem de declaraçao)

EXPLICANDO UM POUCO MELHOR OS METODOS
collect()           eh a primeira passagem
check_class()       verifica se a classe eh vazia
check_method()      monta um escopo local e valida o tipo do retorno
check_command()     tem q resolver na ordem q valdis mandou nas especificacoes do trabalho (4.4)
check_type_of()     sintetiza o tipo da expressao de baixo pra cima (atributos sintetizados!)
check_variable()    tambem tem q resolver na ordem q valdis mandou: 1. params de metodo, 2. atributos de classe, 3. atributos herdados
*/
