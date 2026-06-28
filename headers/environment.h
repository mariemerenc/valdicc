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

    /**
     * @brief Inserts a symbol into the current scope.
     * @return true on success; false if the name already exists in this scope.
     */
    bool insert(const std::string& name, const std::string& type, SymbolKind kind, int scope, int line, int column);

    /**
     * @brief Searches for a name starting from the current scope, expanding to the global scope.
     * @return Symbol* The found symbol, or nullptr if undeclared.
     */    
    Symbol* lookup(const std::string& name);

    /**
     * @brief Creates a nested scope as a child of the current table and descends into it.
     * @param label readable scope name (e.g. "classe X", "método y").
     */
    void addTable(const std::string& label);

    /** @brief Returns to the parent scope */
    void voltar();

    /**
     * @brief Prints the whole scope tree (DFS, depth --> indentation).
     * @param out Output stream.
     */
    void print_symbol_table(std::ostream& out);

    /**
     * @brief Current scope depth, derived from the scope-tree position.
     * @return int scope level of the current table.
     */
    int get_scope();
};

#endif