#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <unordered_set>

#include "../headers/type_checker.h"

using namespace std;
using namespace node_types;
using enum ASTNode::NodeRule;
using enum PrimaryAccessExpr::PEModifier;

TypeChecker::TypeChecker(ASTNode* r) : root(r) {}

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
            if(var->real_var_type->real_type == Type::type_kind::CLASS_TYPE){
                auto class_var = dynamic_cast<ClassType*>(var->real_var_type);
                auto class_fields = class_attributes[class_var->class_name];
                if(class_fields.empty()){
                    throw_semantic_error(var, "tipo está sendo usado antes da definição: " + var->var_id + " tipo: " + class_var->class_name);
                }
            }
            class_attributes[cl->class_id][var->var_id] = var->real_var_type;

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

bool TypeChecker::compatible(string expected, string found){
    if(expected == found){
        return true;
    }
    // se for subclasse
    auto cl = found;

    while(class_parent.count(cl) > 0){
        cl = class_parent[cl];

        if(cl == expected){
            return true;
        }
    }
    return false;
}

string TypeChecker::lookup_method_key(string cl, string met){
    string c = cl;

    while(class_attributes.count(c) > 0){
        string chave = c + "." + met;

        //se tiver ocorrencia dessa chave no mapa de retorno de metodo
        if(method_return.count(chave) > 0){
            return chave;
        }

        // se nao tem superclasse
        if(class_parent.count(c) == 0){
            break;
        }

        c = class_parent[c];
    }

    return "";
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

void TypeChecker::check_method(node_types::MethodDecl* met){
    if(!type_exists(met->methodtype)){
        throw_semantic_error(met, "tipo do retorno n existe: " + met->methodtype);
    }

    current_method_vars.clear();

    //percorrer cada parametro da lista
    for(auto& un_ptr_p : met->param_list){
        auto param = dynamic_cast<VarDecl*>(un_ptr_p.get());

        // caso um parametro esteja duplicado 
        if(current_method_vars.count(param->var_id) > 0){
            throw_semantic_error(param, "parametro duplicaod: " + param->var_id);
        }

        // caso nao exista o tipo do parametro
        if(!type_exists(param->var_type)){
            throw_semantic_error(param, "tipo " + param->var_type + " nao existe");
        }

        // ai se der bom coloca na lista
        current_method_vars[param->var_id] = param->var_type;
    }

    //agora pra cada variavel do metodo
    for(auto& un_ptr_lv : met->local_vars){
        auto lvar = dynamic_cast<VarDecl*>(un_ptr_lv.get());

        // se houver redeclaraçao de variavel
        if(current_method_vars.count(lvar->var_id) > 0){
            throw_semantic_error(lvar, "redeclaracao da variavel " + lvar->var_id);
        }

        // se nao existe o tipo
        if(!type_exists(lvar->var_type)){
            throw_semantic_error(lvar, "tipo " + lvar->var_type + " nao existe");
        }

        current_method_vars[lvar->var_id] = lvar->var_type;
    }

    for(auto& un_ptr_cmd : met->commands_list){
        check_command(un_ptr_cmd.get());
    }

    auto return_type = check_type_of(met->return_expr.get());

    if(!compatible(met->methodtype, return_type)){
        throw_semantic_error(met, "retorno incompativel... recebeu " + return_type + ", esperava " + met->methodtype);
    }
}

string TypeChecker::check_variable(string v_id, ASTNode* location){
    // se for parametro OU variavel local do metodo atual
    if(current_method_vars.count(v_id) > 0){
        return current_method_vars[v_id];
    }
    
    auto curr_cl = current_class;

    // enquanto tiver registro da classe atual no mapa
    while(class_attributes.count(curr_cl) > 0){
        if(class_attributes[curr_cl].count(v_id) > 0){
            if(location->node_rule == IDLITERAL){
                    auto lit_node = dynamic_cast<IdLiteral*>(location);
                if(lit_node->type == nullptr){
                    lit_node->type = class_attributes[curr_cl][v_id];
                }
                else{
                    std::cout << "Não era pra ter chegado aqui. Se o literal não tem o tipo definido, o tipo dele era pra ser nullptr\n";
                    throw_semantic_error(location, "wtf");
                }
            }
            
            return class_attributes[curr_cl][v_id]->get_type_as_string();
        }

        // se a classe atual nao tem superclasse
        if(class_parent.count(curr_cl) == 0){
            break;
        }

        curr_cl = class_parent[curr_cl];
    }

    //se nao retornou antes nao encontrou declaraçao da variavel
    throw_semantic_error(location, "var nao declarada: " + v_id);
    return "";
}

string TypeChecker::check_type_of(ExprNode* expr){
    switch(expr->node_rule){
        case NUMLITERAL:
            return "int";

        case TRUEFALSELITERAL:
            return "boolean";

        case IDLITERAL:{
            auto lit_node = dynamic_cast<IdLiteral*>(expr);
            return check_variable(lit_node->id, expr);
        }
        case THISEXPR:{
            auto this_node = dynamic_cast<ThisExpr*>(expr);
            if (this_node->type == nullptr){
                //TODO!
                throw_semantic_error(expr, "the expression this is not implemented");
            }
            return current_class;
        }

        // 4.1.1
        case ADDEXPR: {
            AddExpr* add_e = dynamic_cast<AddExpr*>(expr);
            //se + ou - for entre nao ints
            if(check_type_of(add_e->lhs.get()) != "int" || check_type_of(add_e->rhs.get()) != "int"){
                throw_semantic_error(expr, "ops aritmeticas (aqui eh soma ou subtraçao) devem ser entre inteiros");
            }
            add_e->type = new IntType(0);
            return "int";
        }

        // 4.1.1
        case MULDIVEXPR:{
            MulDivExpr* mul_e = dynamic_cast<MulDivExpr*>(expr);
            //se * for entre nao ints
            if(check_type_of(mul_e->lhs.get()) != "int" || check_type_of(mul_e->rhs.get()) != "int"){
                throw_semantic_error(expr, "ops aritmeticas (aqui eh multiplicacao) devem ser entre inteiros");
            }
            mul_e->type = new IntType(0);
            return "int";
        }

        // 4.1.2
        case RELEXPR: {
            RelExpr* rel_e = dynamic_cast<RelExpr*>(expr);
            // se > nao for entre ints
            if(check_type_of(rel_e->lhs.get()) != "int" || check_type_of(rel_e->rhs.get()) != "int"){
                throw_semantic_error(expr, "op > deve ser entre inteiros");
            }
            rel_e->type = new BooleanType(false);
            return "boolean";
        }

        // 4.1.3
        case ANDEXPR: {
            AndExpr* and_e = dynamic_cast<AndExpr*>(expr);
            // se && nao for entre bools
            if(check_type_of(and_e->lhs.get()) != "boolean" || check_type_of(and_e->rhs.get()) != "boolean"){
                throw_semantic_error(expr, "op && deve ser entre bools");
            }
            and_e->type = new BooleanType(false);
            return "boolean";
        }

        // 4.1.4
        case NEGATEEXPR: {
            NegateExpr* neg_e = dynamic_cast<NegateExpr*>(expr);
            // se ! nao for com bool
            if(check_type_of(neg_e->lhs.get()) != "boolean"){
                throw_semantic_error(expr, "op ! espera um bool");
            }
            neg_e->type = new BooleanType(false);
            return "boolean";
        }

        // 4.6 ...?
        case NEWOBJEXPR: {
            NewObjExpr* new_obj_e = dynamic_cast<NewObjExpr*>(expr);
            // se nao houver ocorrencias dessa classe no mapa
            if(class_attributes.count(new_obj_e->class_id) == 0){
                throw_semantic_error(expr, "nao existe a classe " + new_obj_e->class_id);
            }
            new_obj_e->type = new ClassType(new_obj_e->class_id, class_attributes[new_obj_e->class_id]);
            return new_obj_e->class_id;
        }

        case NEWARRAYEXPR: {
            NewArrayExpr* new_arr_e = dynamic_cast<NewArrayExpr*>(expr);
            // se o tamanho do vetor nao for int
            if(check_type_of(new_arr_e->size_expr.get()) != "int"){
                throw_semantic_error(expr, "o tamanho do vetor nao eh do tipo int");
            }
            new_arr_e->type = new IntArrayType(0);
            new_arr_e->size_expr->type = new IntType(0);
            return "int[]";
        }
        
        case PRIMARYEXPR: {
            return check_type_of(dynamic_cast<PrimaryExpr*>(expr)->expr.get());
        }

        // 4.1.5 
        case PRIMARYACCESSEXPR:{
            PrimaryAccessExpr* psf_e = dynamic_cast<PrimaryAccessExpr*>(expr);

            auto base = check_type_of(psf_e->lhs.get());

            // se for acesso a array
            if(psf_e->expr_kind == ARRAY_ACCESS){
                // se nao for array
                if(base != "int[]"){
                    throw_semantic_error(expr, "acesso indexado em obj q nao eh vetor");
                }
                // se o indice nao for int
                if(check_type_of(psf_e->access_expr[0].get()) != "int"){
                    throw_semantic_error(expr, "indice do array deve ser int");
                }
                psf_e->type = new IntType(0);
                psf_e->access_expr[0]->type = new IntType(0);
                return "int";
            }

            // se for .length
            else if(psf_e->expr_kind == LENGTH){
                // se nao tem length a ser acessado
                if(base != "int[]"){
                    throw_semantic_error(expr, "chamou .length em nao vetor");
                }
                psf_e->type = new IntType(0);
                return "int";
            }

            // se for chamada .metodo()
            else if(psf_e->expr_kind == METHOD_CALL){
                // se chamou metodo em tipo nao classe
                if(base == "int" || base == "boolean" || base == "int[]"){
                    throw_semantic_error(expr, "chamou metodo em um nao objeto");
                }

                string key = lookup_method_key(base, psf_e->method_id);

                // se procurou metodo e nao achou
                if(key == ""){
                    throw_semantic_error(expr, "metodo " + psf_e->method_id + " nao existe");
                }

                auto& expected_param_types = method_params[key];

                // se o numero de params difere
                if(psf_e->list_expression.size() != expected_param_types.size()){
                    throw_semantic_error(expr, "numero de args recebidos difere do esperado");
                }

                // p cada um dos tipos de params esperados
                for(size_t i = 0; i < expected_param_types.size(); i++){
                    // se o tipo esperado nao for compativel com o tipo recebido
                    if(!compatible(expected_param_types[i], check_type_of(psf_e->list_expression[i].get()))){
                        if(psf_e->list_expression[i]->type == nullptr){
                            psf_e->list_expression[i]->type = Type::str_to_real_type(expected_param_types[i]);
                        }
                        throw_semantic_error(expr, "tipo do argumento recebido nao eh compativel com o esperado");
                    }
                }

                // ai se passou por tudo isso e nao deu erro, retorna o tipo do retorno
                psf_e->type = Type::str_to_real_type(method_return[key]);
                return method_return[key];
            }
        }
        case ASTNode::NodeRule::PROG:
        case ASTNode::NodeRule::MAINDECL:
        case ASTNode::NodeRule::CLASSDECL:
        case ASTNode::NodeRule::VARDECL:
        case ASTNode::NodeRule::METHODDECL:
        case ASTNode::NodeRule::COMMANDDECL:
        case ASTNode::NodeRule::ASSIGNDECL:
        case ASTNode::NodeRule::IFELSEDECL:
        case ASTNode::NodeRule::WHILEDECL:
        case ASTNode::NodeRule::PRINTLN:
          return "";
        }
    return "";
}


void TypeChecker::check_command(ASTNode* cmd){
    switch(cmd->node_rule){
        case ASSIGNDECL: {
            AssignDecl* a = dynamic_cast<AssignDecl*>(cmd);

            string target_type = check_variable(a->lhs_id, a);
            if(a->is_array){
                if(target_type != "int[]"){
                    throw_semantic_error(a, "tentou indexar um nao vetor");
                }

                if(check_type_of(a->index_expr.get()) != "int"){
                    throw_semantic_error(a, "indice deve ser int");
                }

                if(check_type_of(a->rhs.get()) != "int"){
                    throw_semantic_error(a, "rhs da declaraçao deve ser um valor int");
                }
            }
            else{
                string rhs_exp_type = check_type_of(a->rhs.get());

                if(!compatible(target_type, rhs_exp_type)){
                    throw_semantic_error(a, "atribuicao recebeu tipos incompativeis");
                }
            }

            break;
        }

        case IFELSEDECL: {
            IfElseDecl* ie = dynamic_cast<IfElseDecl*>(cmd);

            if(check_type_of(ie->if_exp.get()) != "boolean"){
                throw_semantic_error(cmd, "condiçao do if precisa ser bool");
            }

            for(auto& ic : ie->command_list){
                check_command(ic.get());
            }

            if(ie->has_else){
                for(auto& ec : ie->else_command_list){
                    check_command(ec.get());
                }
            }

            break;
        }

        case WHILEDECL: {
            WhileDecl* w = dynamic_cast<WhileDecl*>(cmd);

            if(check_type_of(w->while_exp.get()) != "boolean"){
                throw_semantic_error(cmd, "a condicao do while precisa ser bool");
            }

            for(auto& c : w->command_list){
                check_command(c.get());
            }
            
            break;
        }

        // chama check_type_of para o tipo de print so p ver se n da erro 
        case PRINTLN: {
            PrintLn* p = dynamic_cast<PrintLn*>(cmd);
            check_type_of(p->print_exp.get());

            break;
        }

        default: {

        }
    }
}