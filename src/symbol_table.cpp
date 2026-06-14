#include "../headers/symbol_table.h"
#include "../headers/environment.h"
#include <stdexcept>

SymbolTable::SymbolTable(SymbolTable * prev_table) : prev{prev_table} {};

SymbolTable::~SymbolTable(){
    for(SymbolTable* f : filhos){
        delete f;
    }
}

SymbolTable * SymbolTable::getPrev() {
    return prev;
}

std::unordered_map<std::string, Symbol> SymbolTable::getSymbolTable() {
    return symbol_table;
}

std::vector<SymbolTable *> SymbolTable::getFilhos(){
    return filhos;
}

void SymbolTable::addFilhos(SymbolTable * filho) {
    filhos.push_back(filho);
}

bool SymbolTable::insert(const std::string& name, const std::string& type, SymbolKind kind, int scope, int line, int column) {
    if (symbol_table.count(name) == 0) { // n existe
        symbol_table[name] = Symbol(name, type, kind, scope, line, column);
        return true;
    }
    
    return false; // ja existe
}

Symbol * SymbolTable::lookup(const std::string& name) {
    if (symbol_table.count(name) != 0) {
        return &symbol_table[name];
    }
    return nullptr;
}