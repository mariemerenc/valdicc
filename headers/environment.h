#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <unordered_map>
#include <string>
#include "symbol.h"
#include "symbol_table.h"
#include <vector>
#include <ostream>

class Environment{
    private:
    SymbolTable * root;
    SymbolTable * curr;

    public:
    Environment();

    ~Environment();

    bool insert(const std::string& name, const std::string& type, SymbolKind kind, int scope, int line, int column); // bool pq vai retornar false se por exemplo uma mesma variavel for declarada duas vezes no mesmo escopo 

    Symbol* lookup(const std::string& name); //eh a funcao de busca na tabela

    // adiciona novas tabelas de simbolos tal qual uma árvore
    void addTable(const std::string& label);

    void voltar();

    void print_symbol_table(std::ostream& out); //metodo p printar a tabela

    int get_scope();
};

#endif