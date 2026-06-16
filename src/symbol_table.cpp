#include "../headers/symbol_table.h"
#include "../headers/environment.h"
#include <stdexcept>
#include <string>
#include <ostream>
#include <algorithm>
#include <vector>

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

void SymbolTable::print_local_symbol_table(int indent, std::ostream& out){
    std::string text_indent(indent*4, ' ');
    
    out << text_indent << "[ESCOPO NÍVEL " << indent << "]\n"; 

    if(symbol_table.empty()){
        out << text_indent << "~vazio~\n";
        return;
    }

    std::vector<Symbol> sort_symb;

    for(auto pair : symbol_table){
        sort_symb.push_back(pair.second);
    }
    std::sort(sort_symb.begin(), sort_symb.end(), [](Symbol& a, Symbol& b){
        if(a.line == b.line){
            return a.column < b.column;
        }
        return a.line < b.line;
    });


    for(auto symb : sort_symb){
        std::string kind_str;

        switch(symb.kind){
            case SymbolKind::CLASS:
                kind_str = "Class";
                break;
            case SymbolKind::METHOD:
                kind_str = "Method";
                break;
            case SymbolKind::VARIABLE:
                kind_str = "Variable";
                break;
            default:
                kind_str = "????";
                break;
        }

        out << text_indent << "Lexeme: " << symb.name
            << " | Type: " << symb.type
            << " | Kind: " << kind_str
            << " | Scope: " << symb.scope
            << " | Line: " << symb.line
            << " | Column: " << symb.column << '\n';
    }
}