#include "../headers/symbol_table.h"
#include "../headers/environment.h"
#include <stdexcept>

Environment::Environment() {
    root = new SymbolTable(nullptr);
    curr = root;
    scope = 0;
}

Environment::~Environment(){
    delete root;
}

void Environment::addTable() { // ativar com base no escopo or something
    SymbolTable * novo = new SymbolTable(curr);

    (*curr).addFilhos(novo);
    curr = novo;
    scope++;
}

void Environment::voltar() {
    if(curr != nullptr && (*curr).getPrev() != nullptr){
        curr = (*curr).getPrev();
        scope--;
    }   
}

bool Environment::insert(const std::string& name, const std::string& type, SymbolKind kind, int scope, int line, int column) {

    return (*curr).insert(name, type, kind, scope, line, column);

}

Symbol * Environment::lookup(const std::string& name) {
    for (auto a = curr; ; a = (*a).getPrev()) {

        if ((*a).lookup(name) != nullptr) {
            return (*a).lookup(name);
        }

        if (a == root) {
            break;
        }
    }
    
    return nullptr;
}

int Environment::get_scope(){
    return scope;
}