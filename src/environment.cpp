#include "../headers/symbol_table.h"
#include "../headers/environment.h"
#include <stdexcept>

Environment::Environment() {
    pai = new SymbolTable();
    curr = pai;
};

void Environment::addTable() { // ativar com base no escopo or something
    SymbolTable * novo = new SymbolTable;
    
    (*curr).addFilhos(novo);
    curr = novo;
}

void Environment::voltar() {
    curr = (*curr).getPrev();
}

bool Environment::insert(const std::string& name, const std::string& type, SymbolKind kind, int scope, int line, int column) {

    return (*curr).insert(name, type, kind, scope, line, column);
    
}

Symbol * Environment::lookup(const std::string& name) {
    for (auto a = curr; ; a = (*a).getPrev()) {

        if ((*a).getSymbolTable().count(name) != 0) {
            return &(*a).getSymbolTable()[name];
        }

        if (a == pai) {
            break;
        }
    }
    
    
}