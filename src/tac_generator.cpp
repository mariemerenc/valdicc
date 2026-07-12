#include "../headers/tac_generator.h"
#include <sstream>

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
            
            quad q(oper::AND, a.back().res, b.back().res, t + (std::to_string(temp_var_counter++)));
            
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

            quad q(oper::NUM, "", "", std::to_string(valor));
            
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
            if (n->is_array) { // do something here
                if(n->index_expr != nullptr) a = generate(n->index_expr.get());
                // VERIFICAR SE ISSO SO ACONTECE PARA ARRAY
                // colocar um retorno aqui also i think
            }

            if(n->rhs != nullptr) b = generate(n->rhs.get());

            // eu preciso estar adicionando mais um quad aqui
            
            quad q(oper::ASS, b.back().res, "", n->lhs_id);

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

            a.push_back(quad(oper::GOTOIF, a.back().res, "", std::to_string(line_counter+1)));
            line_counter++;

            for(auto& com : n->command_list){
                c.splice(c.end(), generate(com.get())); // adicionando recursivamente todos os comandos do if
            }

            // isso porque o line counter ja esta sendo implementado nas folhas
            a.push_back(quad(oper::GOTO, "", "", std::to_string(line_counter + 1)));

            line_counter++;
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

            int pera = line_counter;

            std::list<quad> a = {}, b = {}, c = {};
            
            /*
            1 - calcular a expressao do while, adicionar
            2 - calcular as expressoes dentro do while
            3 - fazer o if goto line_counter + 2 or something (guardar o line counter de onde eu to como temp)
            3 - fazer o goto line_counter + size( das expressoes dentro do while) + 1
            4 - colocar todas as expressoes de dentro do while
            5 - colocar um goto pro inicio
            */
            
            if(n->while_exp != nullptr) c = generate(n->while_exp.get()); // adicionando recursivamente todos os comandos do if

            line_counter+=c.size();

            for(auto& com : n->command_list){
                a.splice(a.end(), generate(com.get())); // adicionando recursivamente todos os comandos do if
            }

            //int lembrar = line_counter;

            c.push_back(quad(oper::GOTOIF, c.back().res, "", std::to_string(line_counter + a.size() + 1)));

            line_counter+=a.size() + 1;

            c.splice(c.end(), a);

            c.push_back(quad(oper::GOTO, "", "", std::to_string(pera)));

            line_counter++;


            break;
        }


        case ASTNode::NodeRule::PROG: {
            
            break;
        }
        case ASTNode::NodeRule::MAINDECL: {
            
            break;
        }
        case ASTNode::NodeRule::CLASSDECL: {
            
            break;
        }
        case ASTNode::NodeRule::VARDECL: {

            // nao faz nada pois a declaracao ja nao importa para nos
            
            break;
        }
        case ASTNode::NodeRule::METHODDECL: {
            
            break;
        }
        case ASTNode::NodeRule::COMMANDDECL: { // isso aq eh oq hein
            
            break;
        }
        case ASTNode::NodeRule::PRINTLN: {
            
            break;
        }
        case ASTNode::NodeRule::PRIMARYACCESSEXPR: { 
            
            break;
        }
        case ASTNode::NodeRule::PRIMARYEXPR: { 
            
            break;
        }
        case ASTNode::NodeRule::THISEXPR: { 
            
            break;
        }
        case ASTNode::NodeRule::NEWOBJEXPR: { 
            
            break;
        }
        case ASTNode::NodeRule::NEWARRAYEXPR: { 
            
            break;
        }

    }


/*


I GOTTA: chegar as lists dos filhos
CALCULAR O NOVO QUAD PARA COLOCAR NO FIM DA LISTA
preciso estar retornando a mais de cima para ser retornada


*/

    
}

