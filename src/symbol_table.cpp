#include "../headers/symbol_table.h"
#include "../headers/environment.h"
#include <stdexcept>
#include <string>
#include <ostream>
#include <algorithm>
#include <vector>
#include <iomanip>

SymbolTable::SymbolTable(SymbolTable * prev_table, const std::string& label) : prev{prev_table}, scope_label{label} {};

SymbolTable::~SymbolTable(){
    for(SymbolTable* f : filhos){
        delete f;
    }
}

SymbolTable * SymbolTable::getPrev() {
    return prev;
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
    
    out << text_indent << "[" << scope_label << "] " << "(escopo " << indent << ")"; 

    if(symbol_table.empty()){
        out << '\n' << text_indent << "~vazio~\n";
        return;
    }
    out << '\n';

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


    for(size_t i=0; i<sort_symb.size(); i++){
        Symbol symb = sort_symb[i];
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

        std::string branch = (i+1 == sort_symb.size()) ? "└─ " : "├─ ";

        out << text_indent << branch << std::left
            << std::setw(15) << symb.name // 15 eh um otimo tamanho max para lexemas.
            << " | Type: " << std::setw(8) << symb.type
            << " | Kind: " << std::setw(8) << kind_str // variable tem 8 letras, method tem 6 e class tem 5
            << " | Line: " << std::setw(3) << symb.line // n acho q vai ter um arquivo com +999 linhas de codigo
            << " | Column: " << std::setw(3) << symb.column << '\n';
    }
}