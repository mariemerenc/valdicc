#include "../headers/symbol_table.h"
#include "../headers/environment.h"
#include <stdexcept>
#include <stack>
#include <utility>

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

void Environment::print_symbol_table(std::ostream& out){
    out << "ÁRVORE DA TABELA DE SÍMBOLOS\n\n";

    if(root == nullptr){
        return;
    }

    std::stack<std::pair<SymbolTable*, int>> p;

    p.push({root, 0});

    while(!p.empty()){
        auto [curr_node, scope] = p.top();
        p.pop();

        (*curr_node).print_local_symbol_table(scope, out);
        
        std::vector<SymbolTable*> filhos = (*curr_node).getFilhos();

        for(auto it = filhos.rbegin(); it != filhos.rend(); it++){
            p.push({*it, scope+1});
        }

        out << '\n';
    }
}