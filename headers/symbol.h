#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>

enum class SymbolKind {
    VARIABLE,
    // METHOD,
    // CLASS
    // etc...?
};

struct Symbol{
    std::string name; // o nome exato da string dada pro simbolo
    std::string type; // tipo do dado int bool int[] 
    SymbolKind kind; // se eh variavel, classe, metodo etc. no momento so tem variavel
    int scope; // acho interessante ja deixar o escopo ai
    int line;
    int column;

    Symbol(const std::string& n, const std::string& t, SymbolKind k, int s, int l, int c){
        name = n;
        type = t;
        kind = k;
        scope = s;
        line = l;
        column = c;
    }

    Symbol(){
        scope = 0;
        line = 0;
        column = 0;
    }
};

#endif