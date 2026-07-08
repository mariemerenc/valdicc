#include <stdexcept>
#include <memory>
#include <unordered_set>

#include "../headers/type_checker.h"

using namespace std;
using namespace node_types;

void TypeChecker::throw_semantic_error(ASTNode* node, const std::string& msg){
    string full_msg = "[ERRO SEMÂNTICO] Na linha " +
                    to_string(node->line) +
                    ", coluna " +
                    to_string(node->column) +
                    ": " +
                    msg;
    
    throw runtime_error(full_msg);
}

void TypeChecker::collect(){
    auto prog = dynamic_cast<ProgNode*>(root);
    auto main = dynamic_cast<MainDecl*>(prog->main_decl.get());

    class_attributes[main->main_class_id]; //vazio p nao dar erro

    //para cada classe de prog
    for(auto& un_ptr_cl : prog->class_decl){
        auto *cl = dynamic_cast<ClassDecl*>(un_ptr_cl.get());

        // se classe ja existe, erro
        if(class_attributes.count(cl->class_id) > 0){
            throw_semantic_error(cl, "classe duplicada: " + cl->class_id);
        }

        class_attributes[cl->class_id]; //vazio p nao dar erro

        // se eh subclasse
        if(cl->extends){
            class_parent[cl->class_id] = cl->inherit_id;
        }

        //para cada variavel da classe
        for(auto& un_ptr_v : cl->variables){
            auto *var = dynamic_cast<VarDecl*>(un_ptr_v.get());

            // se encontrar alguma ocorrencia do nome dessa var na classe atual, erro
            if(class_attributes[cl->class_id].count(var->var_id) > 0){
                throw_semantic_error(var, "atributo duplicado: " + var->var_id);
            }

            class_attributes[cl->class_id][var->var_id] = var->var_type;
        }

        // para cara metodo da classe
        for(auto& un_ptr_m : cl->methods){
            auto *met = dynamic_cast<MethodDecl*>(un_ptr_m.get());
            auto chave = cl->class_id + "." + met->method_id; // so p salvar no mapa tipo Classe.metodo, ja que duas classes podem ter um mesmo nome para metodo

            // se encontrar alguma ocorrencia de Classe.metodo, erro
            if(method_return.count(chave) > 0){
                throw_semantic_error(met, "metodo duplicado na classe: " + met->method_id);
            }

            vector<string> params_type;

            // para cada parametro do metodo
            for(auto& un_ptr_param : met->param_list){
                auto *param = dynamic_cast<VarDecl*>(un_ptr_param.get());

                params_type.push_back(param->var_type);
            }

            method_params[chave] = params_type;
            method_return[chave] = met->methodtype;
        }
    }
}

// tem um check so pra main pq a regra de produçao dele eh mais especifica nao quis mto trabalho ent ta ai
void TypeChecker::check_main(ASTNode* m_node){
    auto main = dynamic_cast<MainDecl*>(m_node);

    current_class = main->main_class_id;
    current_method_vars.clear(); 
    current_method_vars[main->args_var_id] = "String[]"; 

    for(auto& cmd : main->commands){
        check_command(cmd.get());
    }
}

void TypeChecker::check_inheritance(){
    auto prog = dynamic_cast<ProgNode*>(root);

    for(auto& un_ptr_cl : prog->class_decl){
        auto *cl = dynamic_cast<ClassDecl*>(un_ptr_cl.get());

        if(!cl->extends){
            continue;
        }

        unordered_set<string> visitado;
        auto curr = cl->class_id;

        while(class_parent.count(curr) > 0){
            if(visitado.count(curr) > 0){
                throw_semantic_error(cl, "herança circular!!! classe " + cl->class_id);
            }
            visitado.insert(curr);
            curr = class_parent[curr];
        }
    }
}

bool TypeChecker::type_exists(string str){
    if(str == "int" || str == "boolean" || str == "int[]"){
        return true;
    }

    string base = str;
    
    // se for tipo array de classe aaaaaa[] fica aaaaaa so a classe
    if(base.ends_with("[]")){
        base.erase(base.size()-2, 2);
    }

    return class_attributes.count(base) > 0; // e ai retorna se aaaaaa ta no mapa de classes ou n (ou seja se aaaaaa eh um possivel tipo ou nao)
}

void TypeChecker::check(){
    collect();
    check_inheritance(); // precisamos resolver herança ciclica !! tipo class A extends A 

    auto prog = dynamic_cast<ProgNode*>(root);
    
    check_main(prog->main_decl.get());

    for(auto& un_ptr_cl : prog->class_decl){
        auto *cl = dynamic_cast<ClassDecl*>(un_ptr_cl.get());

        check_class(cl);
    }
}

void TypeChecker::check_class(node_types::ClassDecl* cl){
    current_class = cl->class_id;

    // SE A CLASSE TA VAZIA isto eh sem variaveis nem metodos 
    if(cl->variables.empty() && cl->methods.empty()){
        throw_semantic_error(cl, "classe " + cl->class_id + " vazia !");
    }

    // se eh subclasse e o id da superclasse nao existe ta errado !
    if(cl->extends && class_attributes.count(cl->inherit_id) == 0){
        throw_semantic_error(cl, "cade a superclasse? " + cl->inherit_id + " nao existe !");
    }

    // p cada variavel da classe
    for(auto& un_ptr_v : cl->variables){
        auto var = dynamic_cast<VarDecl*>(un_ptr_v.get());

        //se nao tiver esse tipo
        if(!type_exists(var->var_type)){
            throw_semantic_error(var, "tipo " + var->var_type + " nao existe");
        }
    }

    // p cada metodo da classe
    for(auto& un_ptr_m : cl->methods){
        auto method = dynamic_cast<MethodDecl*>(un_ptr_m.get());

        check_method(method);
    }
}