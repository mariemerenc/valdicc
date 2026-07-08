#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <unordered_map>
#include <string>
#include "symbol.h"
#include <vector>

/**
 * @class SymbolTAble
 * @brief A single lexical scope, node of the scope tree
 * 
 * Holds the symbols declared directly in this scope and links to
 * the enclosing scope and the nested scopes, forming an n-ary tree
 * managed by Environment
 */
class SymbolTable{
    private:
    SymbolTable * prev;                                     ///< Enclosing scope
    std::vector <SymbolTable *> filhos;                     ///< Nested scopes
    std::unordered_map<std::string, Symbol> symbol_table;   ///< Symbols declared in this scope
    std::string scope_label;                                ///< Readable description of the scope

    public:
    SymbolTable(SymbolTable * prev_table, const std::string& label);

    ~SymbolTable();

    /**
     * @brief Returns the enclosing scope.
     * @return SymbolTable* The parent, or nullptr at the global scope
     */
    SymbolTable * getPrev();

    /**
     * @brief Returns the scopes nested directly under this one.
     * @return std::vector<SymbolTable*> The child scopes
     */
    std::vector<SymbolTable *> getFilhos();

    /**
     * @brief Registers a nested child scope.
     * @param filho The child scope to attach
     */
    void addFilhos(SymbolTable * filho);

    /**
     * @brief Inserts a symbol into this scope.
     * @param name symbol's identifier
     * @param type declared type (e.g. int, int[], bool, a class name)
     * @param kind symbol kind (variable, method, class)
     * @param scope scope depth where it is declared
     * @param line source line of the declaration
     * @param column source column of the declaration
     * @return true on success; 
     * @return false if the name already exists in this scope.
     */
    bool insert(const std::string& name, const std::string& type, SymbolKind kind, int scope, int line, int column);

    /**
     * @brief Looks a name up in this scope only.
     * @param name identifier to search for
     * @return Symbol* The found symbol, or nullptr if not present
     */
    Symbol* lookup(const std::string& name);

    /**
     * @brief Prints this scope's symbols, ordered by line then column.
     * @param indent scope depth used for indentation
     * @param out output stream
     */
    void print_local_symbol_table(int indent, std::ostream& out); 
};

#endif