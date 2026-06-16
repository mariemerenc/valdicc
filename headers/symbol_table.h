#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <unordered_map>
#include <string>
#include "symbol.h"
#include <vector>

class SymbolTable{
    private:
    SymbolTable * prev;
    std::vector <SymbolTable *> filhos;
    std::unordered_map<std::string, Symbol> symbol_table; // a tabela de simbolos. eh um mapa em que chave = nome do simbolo, valor = simbolo

    public:
    SymbolTable(SymbolTable * prev_table);

    ~SymbolTable();

    SymbolTable * getPrev();

    std::unordered_map<std::string, Symbol> getSymbolTable();

    std::vector<SymbolTable *> getFilhos();

    void addFilhos(SymbolTable * filho);

    bool insert(const std::string& name, const std::string& type, SymbolKind kind, int scope, int line, int column); // bool pq vai retornar false se por exemplo uma mesma variavel for declarada duas vezes no mesmo escopo 

    Symbol* lookup(const std::string& name); //eh a funcao de busca na tabela

    void print_local_symbol_table(int indent, std::ostream& out); //metodo p printar a tabela
};

#endif