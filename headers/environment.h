#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <unordered_map>
#include <string>
#include "symbol.h"
#include "symbol_table.h"
#include <vector>

class Environment{
    private:
    SymbolTable * pai;
    SymbolTable * curr;

    public:
    Environment();

    bool insert(const std::string& name, const std::string& type, SymbolKind kind, int scope, int line, int column); // bool pq vai retornar false se por exemplo uma mesma variavel for declarada duas vezes no mesmo escopo 

    Symbol* lookup(const std::string& name); //eh a funcao de busca na tabela

    // adiciona novas tabelas de simbolos tal qual uma árvore
    void addTable();

    void voltar();

    void print_symbol_table(); //metodo p printar a tabela
};

#endif