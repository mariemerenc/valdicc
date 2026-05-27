#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>

/**
 * @brief Defines the kind of a symbol within the compiler
 * * Currently supports variables, but is designed to be extensible
 * for other language constructs such as methods and classes.
 */
enum class SymbolKind {
    VARIABLE,
    // METHOD,
    // CLASS
    // etc...?
};

struct Symbol{
    std::string name;   ///< The exact string lexeme of the identifier.
    std::string type;   ///< The data type of the symbol.
    SymbolKind kind;    ///< The classification/kind of the symbol.
    int scope;          ///< The scope depth level where the symbol was declared.
    int line;           ///< The line number in the source file where it was declared.
    int column;         ///< The column number in the source file where it was declared.

    /**
     * @brief Constructs a new Symbol object.
     * 
     * @param n The exact lexeme of the symbol.
     * @param t The data type associated with the symbol.
     * @param k The kind of the symbol. acho interessante ja deixar o escopo ai
     * @param s The scope depth level of the symbol.
     * @param l The line number where the symbol appears.
     * @param c The column number where the symbol appears.
     */
    Symbol(const std::string& n, const std::string& t, SymbolKind k, int s, int l, int c){
        name = n;
        type = t;
        kind = k;
        scope = s;
        line = l;
        column = c;
    }

    /**
     * @brief Default constructor for a Symbol object.
     * * Initializes positional and scope metadata to zero.
     */
    Symbol(){
        scope = 0;
        line = 0;
        column = 0;
    }
};

#endif