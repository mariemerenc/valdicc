#include <stdexcept>
#include <memory>

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