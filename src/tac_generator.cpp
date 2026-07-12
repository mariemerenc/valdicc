#include "../headers/tac_generator.h"
#include <sstream>
#include <stdexcept>
#include "../headers/AST.h"
#include <iostream>


TacGenerator::TacGenerator(ASTNode* rooot) {
    root = rooot;
    final = generate(root);

}


//olha aqui vai ficar faltando os casos de this
//e la no generate, o DEFINE precisa de class_id + method_id pra casar com o CALL
//atualmente estao diferentes pq nao sei como diabos vamos acessar o class_id 
//mas vou atras disso
string TacGenerator::printTac() {
    // lembrar de apenas ignorar os literais (numero, id e truefalse); eles so sao
    // usados para podermos "retornar" sem retornar

    // se a gnt for printar o no. da linha ta aq (pt.1):
    //int n = 0;
    string output = "";
    for(auto& q : final){
        if(q.opp == oper::NUM || q.opp == oper::ID || q.opp == oper::TRUEFALSE){
            continue;
        }
    
        // se a gnt for printar o no. da linha ta aq (pt.2):
        //output += std::to_string(n) + ". ";
        
        switch(q.opp){
            case oper::ADD:{
                output += (q.res + " = " + q.arg1 + " + " + q.arg2 + "\n");
                break;
            }

            case oper::SUB:{
                output += (q.res + " = " + q.arg1 + " - " + q.arg2 + "\n");
                break;
            }

            case oper::MULT:{
                output += (q.res + " = " + q.arg1 + " * " + q.arg2 + "\n");
                break;
            }

            case oper::REL:{
                output += (q.res + " = " + q.arg1 + " > " + q.arg2 + "\n");
                break;
            }

            case oper::AND:{
                output += (q.res + " = " + q.arg1 + " && " + q.arg2 + "\n");
                break;
            }

            case oper::NOT:{
                output += (q.res + " = !" + q.arg1 + "\n");
                break;
            }

            case oper::LEN:{
                output += (q.res + " = " + q.arg1 + ".len\n");
                break;
            }

            case oper::ASS:{
                output += (q.res + " = " + q.arg1 + "\n");
                break;
            }

            case oper::GOTO:{
                output += ("goto " + q.res + "\n");
                break;
            }

            case oper::GOTOIF:{
                output += ("if " + q.arg1 + " goto " + q.res + "\n");
                break;
            }

            case oper::PARAM:{
                output += ("param " + q.arg1 + "\n");
                break;
            }

            case oper::CALL:{
                //output += ("call " + q.arg1 + ", " + q.arg2 + "\n");
                //no livro essa aq debaixo tava dizendo q era "opcional" lembram
                //mas aq nao ta bem assim como o livro sugere enfim 
                output += (q.res + " = call " + q.arg1 + ", " + q.arg2 + "\n");
                break;
            }

            case oper::ACCESS:{
                output += (q.res + " = " + q.arg1 + "[" + q.arg2 + "]\n");
                break;
            }

            case oper::PRINT:{
                output += ("print " + q.arg1 + "\n");
                break;
            }

            case oper::DEFINE:{
                output += ("DEFINE "+q.arg1 + ":\n");
                break;
            }

            case oper::RETURN:{
                output += ("return " + q.arg1 + "\n");
                break;
            }

            case oper::NEWOBJ:{
                output += (q.res + " = new " + q.arg1 + "\n");
                break;
            }

            case oper::NEWARR:{
                output += (q.res + " = new int[" + q.arg1 + "]\n");
                break;
            }

            case oper::THIS:{
                //pensando aq como vamos resolver esse this viu......
                break;
            }

            case oper::HALT:{
                output += ("HALT main\n");
                break;
            }


        }
        // se a gnt for printar o no. da linha ta aq (pt.3):
        //n++;
    }

    return output;
}


string t = "t";
// rever toda a logica do line counter -> acho que preciso adicionar o len ao no depois de visitar os filhos e o no
// ONDE COLOCAR LINE COUNTERRRRRRRRRRRRRR

std::list<quad> TacGenerator::generate(ASTNode* node){

    switch(node->node_rule){

        // ARITHMETIC EXPRESSIONS

        case ASTNode::NodeRule::ADDEXPR: {
            auto* n = dynamic_cast<node_types::AddExpr*>(node);
            std::list<quad> a = {}, b = {};
            if(n->lhs != nullptr) a = generate(n->lhs.get());
            if(n->rhs != nullptr) b = generate(n->rhs.get());

            oper o;
            if (n->op == node_types::AddExpr::Operation::SUM) {
                o = oper::ADD;
            }
            else {
                o = oper::SUB;
            }
            
            quad q(o, a.back().res, b.back().res, t + (std::to_string(temp_var_counter++)));
            line_counter++;
            b.push_back(q);
            a.splice(a.end(), b);
            return a;
            break;
        }

        case ASTNode::NodeRule::MULDIVEXPR: { // nossa linguagem nao tem divisao loll
            auto* n = dynamic_cast<node_types::MulDivExpr*>(node);
            std::list<quad> a = {}, b = {};
            if(n->lhs != nullptr) a = generate(n->lhs.get());
            if(n->rhs != nullptr) b = generate(n->rhs.get());

            // eu preciso estar adicionando mais um quad aqui
            
            quad q(oper::MULT, a.back().res, b.back().res, t + (std::to_string(temp_var_counter++)));
            
            line_counter++;
            b.push_back(q);
            a.splice(a.end(), b);
            return a;
            break;
        }

        // LOGICAL EXPRESSIONS

        case ASTNode::NodeRule::ANDEXPR: {
            auto* n = dynamic_cast<node_types::AndExpr*>(node);
            std::list<quad> a = {}, b = {};
            if(n->lhs != nullptr) a = generate(n->lhs.get());
            if(n->rhs != nullptr) b = generate(n->rhs.get());

            // eu preciso estar adicionando mais um quad aqui
            
            quad q(oper::AND, a.back().res, b.back().res, t + (std::to_string(temp_var_counter++)));
            
            line_counter++;
            b.push_back(q);
            a.splice(a.end(), b);
            return a;
            
            break;
        }

        case ASTNode::NodeRule::NEGATEEXPR: {
            
            auto* n = dynamic_cast<node_types::NegateExpr*>(node);
            std::list<quad> a = {}, b = {};
            if(n->lhs != nullptr) a = generate(n->lhs.get());
            //if(n->rhs != nullptr) b = generate(n->rhs.get());

            // eu preciso estar adicionando mais um quad aqui
            
            quad q(oper::NOT, a.back().res, "", t + (std::to_string(temp_var_counter++)));
            
            line_counter++;
            a.push_back(q);
            return a;


            break;
        }

        case ASTNode::NodeRule::RELEXPR: {
            
            auto* n = dynamic_cast<node_types::RelExpr*>(node);
            std::list<quad> a = {}, b = {};
            if(n->lhs != nullptr) a = generate(n->lhs.get());
            if(n->rhs != nullptr) b = generate(n->rhs.get());

            // eu preciso estar adicionando mais um quad aqui
            
            quad q(oper::REL, a.back().res, b.back().res, t + (std::to_string(temp_var_counter++)));
            
            line_counter++;
            b.push_back(q);
            a.splice(a.end(), b);
            return a;
            


            break;
        }

        // LITERALS


        // o plzno para literais, numeros e variaveis eh colocar na lista de quads (pra termos acesso) e so nao imprimir
        case ASTNode::NodeRule::NUMLITERAL: {
            auto* n = dynamic_cast<node_types::NumLiteral*>(node);
            std::list<quad> a = {}, b = {};

            int64_t valor = n->int_val;

            quad q(oper::NUM, "", "", std::to_string(valor));
            
            a.push_back(q);
            return a;
            
            break;
        }

        case ASTNode::NodeRule::TRUEFALSELITERAL: {

            auto* n = dynamic_cast<node_types::TrueFalseLiteral*>(node);
            std::list<quad> a = {}, b = {};

            int64_t valor = n->bool_val;

            string s = "";
            if (valor == 0) {
                s = "false";
            }
            else {
                s = "true";
            }

            quad q(oper::NUM, "", "", s);
            
            a.push_back(q);
            return a;
            
            break;
        }

        case ASTNode::NodeRule::IDLITERAL: {

            auto* n = dynamic_cast<node_types::IdLiteral*>(node);
            std::list<quad> a = {}, b = {};

            string nome = n->id;

            quad q(oper::NUM, "", "", nome);
            
            a.push_back(q);
            return a;
            
            break;
        }



        case ASTNode::NodeRule::ASSIGNDECL: {
            auto* n = dynamic_cast<node_types::AssignDecl*>(node);
            
            std::list<quad> a = {}, b = {};
            string s = "";
            if (n->is_array) { // do something here
                if(n->index_expr != nullptr) a = generate(n->index_expr.get());
                s += "[" + a.back().res+ "]";
            }

            if(n->rhs != nullptr) b = generate(n->rhs.get());

            // eu preciso estar adicionando mais um quad aqui
            
            quad q(oper::ASS, b.back().res, "", n->lhs_id + s);

            line_counter++;
            b.push_back(q);
            a.splice(a.end(), b);
            return a;
            break;
        }



        // CONDITIONALS

        case ASTNode::NodeRule::IFELSEDECL: {
            // tudo que volta daqui estara ou no if ou no else entao o nosso approach de ordem funciona yes

            auto* n = dynamic_cast<node_types::IfElseDecl*>(node);

            // avalia expressao dentro do if
            // adiciona o quad que faz o goto para a linha logo apos (entra se for true)
            // avalia a lista de comandos que ocorrem caso true 
            // adiciona o goto de caso false para um depois da lista de comandos
            // junta as duas listas de comandos

            std::list<quad> a = {}, b = {}, c = {}, d = {};


            if(n->if_exp != nullptr) a = generate(n->if_exp.get());

            a.push_back(quad(oper::GOTOIF, a.back().res, "", std::to_string(line_counter+2)));
            line_counter++;

            for(auto& com : n->command_list){
                c.splice(c.end(), generate(com.get())); // adicionando recursivamente todos os comandos do if
            }


            if (n->has_else == false){
                a.push_back(quad(oper::GOTO, "", "", std::to_string(line_counter + 1)));
                
            }
            // literalmente adicionando o goto a mais lol
            else {
                a.push_back(quad(oper::GOTO, "", "", std::to_string(line_counter + 2)));
            }

            line_counter++;

            

            // isso porque o line counter ja esta sendo implementado nas folhas


            

            a.splice(a.end(), c);

            if (n->has_else == true) {
                for(auto& com : n->else_command_list){
                    d.splice(d.end(), generate(com.get())); // adicionando recursivamente todos os comandos do if
                }

                a.push_back(quad(oper::GOTO, "", "", std::to_string(line_counter + 1)));
                line_counter ++;
                a.splice(a.end(), d);
            }

            return a;

            break;
        }


        case ASTNode::NodeRule::WHILEDECL: {
            auto* n = dynamic_cast<node_types::WhileDecl*>(node);

            // ATUALMENTE VOLTO PRA RECALCULAR A WHILE_EXP, VERIFICAR SE THATS WHAT I WANNA DO

            
            std::list<quad> a = {}, b = {}, c = {};
            
            /*
            1 - calcular a expressao do while, adicionar
            2 - calcular as expressoes dentro do while
            3 - fazer o if goto line_counter + 2 or something (guardar o line counter de onde eu to como temp)
            3 - fazer o goto line_counter + size( das expressoes dentro do while) + 1
            4 - colocar todas as expressoes de dentro do while
            5 - colocar um goto pro inicio
            */
            int pera2 = line_counter;
            if(n->while_exp != nullptr) c = generate(n->while_exp.get()); // adicionando recursivamente todos os comandos do if
            int pera = line_counter;
           
            //line_counter+=c.size();

            for(auto& com : n->command_list){
                a.splice(a.end(), generate(com.get())); // adicionando recursivamente todos os comandos do if
            }

            //int lembrar = line_counter;

            c.push_back(quad(oper::GOTOIF, c.back().res, "", std::to_string(pera+2)));

            line_counter++;

            c.push_back(quad(oper::GOTO, "", "", std::to_string(line_counter+2)));

            line_counter++;

            c.splice(c.end(), a);

            c.push_back(quad(oper::GOTO, "", "", std::to_string(pera2)));

            line_counter++;

            return c;


            break;
        }


        case ASTNode::NodeRule::PROG: {
            std::list<quad> a = {};
            auto prog = dynamic_cast<node_types::ProgNode*>(node);
            a.splice(a.end(), generate(prog->main_decl.get()));

            for(auto& un_ptr_cl : prog->class_decl){
                auto cl = dynamic_cast<node_types::ClassDecl*>(un_ptr_cl.get());

                a.splice(a.end(), generate(cl));
            }

            return a; 
           
            break;
        }
        case ASTNode::NodeRule::MAINDECL: {

            std::list<quad> a = {};
            auto main = dynamic_cast<node_types::MainDecl*>(node);

            for(auto& cmd : main->commands){
                a.splice(a.end(), generate(cmd.get()));
            }

            a.push_back(quad(oper::HALT, "", "", ""));
            line_counter++;

            return a;
            
            break;
        }
        case ASTNode::NodeRule::CLASSDECL: {
            std::list<quad> a = {};
            auto cl = dynamic_cast<node_types::ClassDecl*>(node);

            for(auto& met : cl->methods){
                a.splice(a.end(), generate(met.get()));
            }

            return a;
            break;
        }
        case ASTNode::NodeRule::METHODDECL: {

            /*
            classe.metodo1
            t1 = 20
            t2 = t1 + 10
            return t2
            */

            // vamos pensar mais nisso aq
            std::list<quad> a = {}, b = {}, c = {};

            auto n = dynamic_cast<node_types::MethodDecl*>(node);

            // imprimir aqui o nome da classe + nome do metodo

            // na minha cabeca faz sentido colocar parametros para serem definidos.

            // pra cada parametro eu defino um param logo depois de define funcao


            //auto pera = dynamic_cast<ClassType*>("");

            // SKEETE COMO COLOCA A CLASSE AQ

            /*auto aaaaa = dynamic_cast<ClassType*>(n->lhs->type);

            if (!aaaaa) {
                throw std::runtime_error("nao conseguimos acessar a classe a qual o metodo se refere (TAC)\n");
                exit(1);
            }*/

            /*DEFINE adicionar
            DEFINE Calculadora.adicionar*/


            a.push_back(quad(oper::DEFINE, n->method_id, "", ""));
            line_counter++;

            for(auto& arg : n->param_list){

                auto p = dynamic_cast<node_types::VarDecl*>(arg.get());
                a.push_back(quad(oper::PARAM, p->var_id, "", ""));
                line_counter++;
                //a.splice(a.end(), generate(met.get()));
            }

            


            for (auto& c : n->commands_list) {
                a.splice(a.end(), generate(c.get()));
            }

            a.splice(a.end(), generate(n->return_expr.get()));

            a.push_back(quad(oper::RETURN, a.back().res, "", ""));
            line_counter++;

            return a;

        
            break;
        }
        case ASTNode::NodeRule::PRINTLN: {
            auto pln = dynamic_cast<node_types::PrintLn*>(node);
            auto a = generate(pln->print_exp.get());

            a.push_back(quad(oper::PRINT, a.back().res, "", ""));
            line_counter++;

            return a;
            break;
        }
        case ASTNode::NodeRule::PRIMARYACCESSEXPR: { 
            auto* n = dynamic_cast<node_types::PrimaryAccessExpr*>(node);

            std::list<quad> a ={}, b = {};

            switch (n->expr_kind){
                case node_types::PrimaryAccessExpr::PEModifier::LENGTH: {
                    
                    
                    

                    node_types::IdLiteral* aaaaa = dynamic_cast<node_types::IdLiteral*>(n->lhs.get());

                    if (!aaaaa) {
                        throw std::runtime_error("nao conseguimos acessar a o nome a qual o acesso se refere 1 (TAC)\n");
                        exit(1);
                    }


                    a.push_back(quad(oper::LEN, aaaaa->id, "", t + std::to_string(temp_var_counter++)));
                    line_counter++;


                    return a;


                    break;
                }

                case node_types::PrimaryAccessExpr::PEModifier::ARRAY_ACCESS: { // PODEMOS MODIFICAR PARA O SIZE CASO SEJA NECESSARIO

                    for(auto& com : n->access_expr){
                        a.splice(a.end(), generate(com.get()));
                    }


                    node_types::IdLiteral* aaaaa = dynamic_cast<node_types::IdLiteral*>(n->lhs.get());
                    if (!aaaaa) {
                        throw std::runtime_error("nao conseguimos acessar a o nome a qual o acesso se refere 2 (TAC)\n");
                        exit(1);
                    }

                    a.push_back(quad(oper::MULT, a.back().res, "8", t + std::to_string(temp_var_counter++)));
                    line_counter++;


                    a.push_back(quad(oper::ACCESS, aaaaa->id, a.back().res, t + std::to_string(temp_var_counter++)));
                    line_counter++;

                    return a;
                    break;
                }

                case node_types::PrimaryAccessExpr::PEModifier::METHOD_CALL: {

                    std::list<string> parametros = {};

                    for(auto& com : n->list_expression){
                        a.splice(a.end(), generate(com.get()));
                        parametros.push_back(a.back().res);

                    }
                    for (string param : parametros) {
                        a.push_back(quad(oper::PARAM, param, "", ""));
                        line_counter++;
                    }

                    auto aaaaa = dynamic_cast<ClassType*>(n->lhs->type);

                    if (!aaaaa) {
                        throw std::runtime_error("nao conseguimos acessar a classe a qual o metodo se refere (TAC)\n");
                        exit(1);
                    }


                    a.push_back(quad(oper::CALL, aaaaa->class_name + "." + n->method_id, std::to_string(parametros.size()), t + std::to_string(temp_var_counter++)));
                    line_counter++;


                    return a;
                    break;
                }

                default: {
                    std::list<quad> a = {};
                    return a;
                    break;
                }
        
            }



            break;
        }
        case ASTNode::NodeRule::PRIMARYEXPR: {

            auto prim = dynamic_cast<node_types::PrimaryExpr*>(node);

            return generate(prim->expr.get());
            break;
        }
        case ASTNode::NodeRule::THISEXPR: { // REVER

            std::list<quad> a = {}, b = {};
            auto prim = dynamic_cast<node_types::ThisExpr*>(node);

            a.push_back(quad(oper::THIS, "eita", "", t + std::to_string(temp_var_counter++)));
            line_counter++;

            return a;

            break;
        }
        case ASTNode::NodeRule::NEWOBJEXPR: { 

            std::list<quad> a = {};
            auto prim = dynamic_cast<node_types::NewObjExpr*>(node);

            a.push_back(quad(oper::NEWOBJ, prim->class_id, "", t + std::to_string(temp_var_counter++)));
            line_counter++;
            return a;
            break;
        }
        case ASTNode::NodeRule::NEWARRAYEXPR: { 
            
            std::list<quad> a = {};
            auto prim = dynamic_cast<node_types::NewArrayExpr*>(node);

            a = generate(prim->size_expr.get()); // nao fazemos ideia se isso vai funcionar

            /*a.push_back(quad(oper::MULT, a.back().res, "8", t + std::to_string(temp_var_counter++)));
            line_counter++; nao sei se precisa multiplicar por 8 tambem*/

            a.push_back(quad(oper::NEWARR, a.back().res, "", t + std::to_string(temp_var_counter++)));
            line_counter++;
            return a;
            break;
        }

        default: {
            std::list<quad> a = {};
            return a;
            break;
        }

    }


/*
I GOTTA: checar as lists dos filhos
CALCULAR O NOVO QUAD PARA COLOCAR NO FIM DA LISTA
preciso estar retornando a mais de cima para ser retornada
*/
std::cout << "eita........\n";
std::cout << node << "\n\n";
return {};
    
}

