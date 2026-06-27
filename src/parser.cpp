#include "../headers/parser.h"
#include "../headers/usefultools.h"
#include <memory>
#include <stdexcept>
#include <limits.h>
#include <vector>
using namespace std;

Parser::Parser(const std::vector<Token>& tokenss, RunningOptions opts) : tokens{tokenss}, m_running_opts{opts} {}

using NodePtr = std::unique_ptr<ASTNode>;
using NodeVec = std::vector<NodePtr>;
using ExprNodePtr = std::unique_ptr<ExprNode>;


Token Parser::peek(){
    if(lookahead >= tokens.size()){
        return tokens.back();
    }
    return tokens[lookahead];
}


Token Parser::peek_next(){
    if(lookahead+1 >= tokens.size()){
        return tokens.back();
    }
    return tokens[lookahead+1];
}


Token Parser::previous(){
    if(lookahead == 0){
        return tokens[0];
    }
    return tokens[lookahead-1];
}

bool Parser::peek_Cmd(){
    if ( peek().type == TokenType::IDENTIFIER ||
            peek().type == TokenType::KW_IF ||
            peek().type == TokenType::KW_WHILE ||
            peek().type == TokenType::KW_SYSTEM){
                return true;
            }

    return false;
}


void Parser::throw_error(const std::string& msg, ErrorPhase phase){
    Token curr_token = peek();

    string label = (phase == ErrorPhase::SEMANTIC) ? "[ERRO SEMÂNTICO]" : "[ERRO SINTÁTICO]";

    string full_msg = label +
                    " na linha " +
                    to_string(curr_token.line) +
                    ", coluna " +
                    to_string(curr_token.column) +
                    ": " +
                    msg;


    if(curr_token.type == TokenType::IDENTIFIER && m_running_opts.suggest_corrections){
        string best_match;
        int best_dist = INT_MAX;

        for(const auto& [keyword, _] : KeywordMap){
            int dist = UFT::levenshtein_distance(curr_token.lexeme, keyword);
            if(dist < best_dist){
                best_dist = dist;
                best_match = keyword;
            }
        }

        if(best_dist <= 2){
            full_msg += ". Você quis dizer: '" + best_match + "'?";
        }
    }
    throw runtime_error(full_msg);
}


void Parser::match(TokenType expected_type, const string& custom_msg){
    if(peek().type == expected_type){
        lookahead++;
    }
    else{
        if(custom_msg.empty()){
            string msg = "Esperava " +
                        to_string(expected_type) +
                        ", recebeu " +
                        to_string(peek().type);

            throw_error(msg);
        }
        else{
            throw_error(custom_msg);
        }
    }
}


AST Parser::parse(){
    AST tree(parse_Prog());

    if(lookahead < tokens.size() && peek().type != TokenType::END_OF_FILE){
        throw_error("Presença de tokens após EOF.");
    }
    return tree;
}


NodePtr Parser::parse_Prog(){
    auto m = parse_MainC();
    auto d = parse_DefCl();
    return make_unique<node_types::ProgNode>(std::move(m), std::move(d));
}


NodePtr Parser::parse_MainC(){
    //class ABC {
    match(TokenType::KW_CLASS);
    match(TokenType::IDENTIFIER);
    Token tkn_class = previous();
    string main_class_id = tkn_class.lexeme;
    env.insert(main_class_id, "class", SymbolKind::CLASS, env.get_scope(), tkn_class.line, tkn_class.column);
    match(TokenType::PUNC_LBRACE);
    env.addTable("classe " + main_class_id);

    //public static void main(String[] args) {
    match(TokenType::KW_PUBLIC);
    match(TokenType::KW_STATIC);
    match(TokenType::KW_VOID);
    match(TokenType::KW_MAIN);
    env.addTable("método main");

    match(TokenType::PUNC_LPARENT);
    match(TokenType::KW_STRING);
    match(TokenType::PUNC_LBRACKET);
    match(TokenType::PUNC_RBRACKET);
    match(TokenType::IDENTIFIER);
    Token tkn_args = previous();
    string args_id = tkn_args.lexeme;
    env.insert(args_id, "String[]", SymbolKind::VARIABLE, env.get_scope(), tkn_args.line, tkn_args.column);
    match(TokenType::PUNC_RPARENT);
    match(TokenType::PUNC_LBRACE);
    auto command_list = parse_Lcom();
    match(TokenType::PUNC_RBRACE);
    env.voltar();
    match(TokenType::PUNC_RBRACE);
    env.voltar();
    return make_unique<node_types::MainDecl>(main_class_id, args_id, std::move(command_list));
}


NodeVec Parser::parse_DefCl(){
    NodeVec class_definitions;
    while(peek().type == TokenType::KW_CLASS){
        match(TokenType::KW_CLASS);
        match(TokenType::IDENTIFIER);
        Token tkn_class = previous();
        string class_id = tkn_class.lexeme;

        bool extends = false;
        string extends_id = "";
        if(peek().type == TokenType::KW_EXTENDS){
            match(TokenType::KW_EXTENDS);
            match(TokenType::IDENTIFIER);
            extends = true;
            extends_id = previous().lexeme;
        }

        bool success = env.insert(class_id, "class", SymbolKind::CLASS, env.get_scope(), tkn_class.line, tkn_class.column);
        if(!success){
            throw_error("Classe já declarada: " + class_id, ErrorPhase::SEMANTIC);
        }

        match(TokenType::PUNC_LBRACE);
        env.addTable("classe " + class_id);
        NodeVec vars(parse_DefVar());
        NodeVec methods(parse_DefMet());
        match(TokenType::PUNC_RBRACE);
        env.voltar();
        class_definitions.push_back(make_unique<node_types::ClassDecl>(class_id, std::move(vars),
                                     std::move(methods), extends, extends_id));
    }
    return class_definitions;
}


NodeVec Parser::parse_DefVar() {
    // Uses lookahead (peek_next) to resolve structural ambiguity.
    NodeVec vars;
    while ( peek().type == TokenType::KW_INT ||
            peek().type == TokenType::KW_BOOLEAN ||
            (peek().type == TokenType::IDENTIFIER && peek_next().type == TokenType::IDENTIFIER)){
        string type = parse_Type();
        match(TokenType::IDENTIFIER);
        Token tkn_id = previous();
        string id = tkn_id.lexeme;

        bool success = env.insert(id, type, SymbolKind::VARIABLE, env.get_scope(), tkn_id.line, tkn_id.column);
        if(!success){
            throw_error("Variável já declarada neste escopo: " + id, ErrorPhase::SEMANTIC);
        }

        match(TokenType::PUNC_SEMICOLON);
        vars.push_back(make_unique<node_types::VarDecl>(id, type));
    }
    return vars;
}


NodeVec Parser::parse_DefMet() {
    NodeVec methods;
    while(peek().type == TokenType::KW_PUBLIC){
        match(TokenType::KW_PUBLIC);
        string type = parse_Type();
        match(TokenType::IDENTIFIER);
        Token tkn_met = previous();
        string id = tkn_met.lexeme;

        bool success = env.insert(id, type, SymbolKind::METHOD, env.get_scope(), tkn_met.line, tkn_met.column);
        if(!success){
            throw_error("Método já declarado: " + id, ErrorPhase::SEMANTIC);
        }

        env.addTable("método " + id);  // tabela antes dos args para parâmetros e DefVar ficarem juntos

        match(TokenType::PUNC_LPARENT);
        NodeVec args;
        if(peek().type != TokenType::PUNC_RPARENT){
            args = parse_Args();
        }
        match(TokenType::PUNC_RPARENT);
        match(TokenType::PUNC_LBRACE);
        NodeVec vars = parse_DefVar();
        NodeVec commands;
        while ( peek().type == TokenType::IDENTIFIER ||
                peek().type == TokenType::KW_IF ||
                peek().type == TokenType::KW_WHILE ||
                peek().type == TokenType::KW_SYSTEM){
            commands = parse_Lcom();
        }
        match(TokenType::KW_RETURN);
        ExprNodePtr expr = parse_Exp();
        match(TokenType::PUNC_SEMICOLON);
        match(TokenType::PUNC_RBRACE);
        env.voltar();
        methods.push_back(make_unique<node_types::MethodDecl>(id, type, std::move(args), std::move(commands), std::move(expr)));
    }
    return methods;
}


string Parser::parse_Type() {
    if (peek().type == TokenType::KW_INT) {
        match(TokenType::KW_INT);
        if (peek().type == TokenType::PUNC_LBRACKET) {
            match(TokenType::PUNC_LBRACKET);
            match(TokenType::PUNC_RBRACKET);
            return "int[]";
        }
        return "int";
    }

    else if (peek().type == TokenType::KW_BOOLEAN) {
        match(TokenType::KW_BOOLEAN);
        return "boolean";
    }

    else {
        match(TokenType::IDENTIFIER);
        return previous().lexeme;
    }
}


NodeVec Parser::parse_Args() {
    NodeVec args;

    string type = parse_Type();
    match(TokenType::IDENTIFIER);
    Token tkn_id = previous();

    bool success = env.insert(tkn_id.lexeme, type, SymbolKind::VARIABLE, env.get_scope(), tkn_id.line, tkn_id.column);
    if(!success){
        throw_error("Tentativa de inserção de parâmetro duplicado: " + tkn_id.lexeme, ErrorPhase::SEMANTIC);
    }
    args.push_back(make_unique<node_types::VarDecl>(tkn_id.lexeme, type));

    while(peek().type == TokenType::PUNC_COMMA){
        match(TokenType::PUNC_COMMA);
        type = parse_Type();
        match(TokenType::IDENTIFIER);
        tkn_id = previous();

        success = env.insert(tkn_id.lexeme, type, SymbolKind::VARIABLE, env.get_scope(), tkn_id.line, tkn_id.column);
        if(!success){
            throw_error("Tentativa de inserção de parâmetro duplicado: " + tkn_id.lexeme, ErrorPhase::SEMANTIC);
        }
        args.push_back(make_unique<node_types::VarDecl>(tkn_id.lexeme, type));
    }
    return args;
}


NodeVec Parser::parse_Lcom(){
    NodeVec commands;
    commands.push_back(parse_Cmd());

    while ( peek().type == TokenType::IDENTIFIER ||
            peek().type == TokenType::KW_IF ||
            peek().type == TokenType::KW_WHILE ||
            peek().type == TokenType::KW_SYSTEM){
        commands.push_back(parse_Cmd());
    }
    return commands;
}


NodePtr Parser::parse_Cmd() {
    if (peek().type == TokenType::KW_IF) {
        match(TokenType::KW_IF); 
        match(TokenType::PUNC_LPARENT);
        auto ifelse_exp = parse_Exp();
        match(TokenType::PUNC_RPARENT);
        match(TokenType::PUNC_LBRACE);
        auto if_cmds = peek_Cmd() ? parse_Lcom() : NodeVec{};
        match(TokenType::PUNC_RBRACE);
        bool has_else = false;
        NodeVec else_cmds = {};

        //I --> else { Lcom } | lambda
        if(peek().type == TokenType::KW_ELSE){
            has_else = true;
            match(TokenType::KW_ELSE);
            match(TokenType::PUNC_LBRACE);
            else_cmds = peek_Cmd() ? parse_Lcom() : NodeVec{};
            match(TokenType::PUNC_RBRACE);
        }

        return make_unique<node_types::IfElseDecl>(std::move(ifelse_exp), std::move(if_cmds), has_else, std::move(else_cmds));
    }
    else if (peek().type == TokenType::KW_WHILE) {
        match(TokenType::KW_WHILE);
        match(TokenType::PUNC_LPARENT);
        auto while_exp = parse_Exp();
        match(TokenType::PUNC_RPARENT);
        match(TokenType::PUNC_LBRACE);
        auto while_cmds = peek_Cmd() ? parse_Lcom() : NodeVec{};
        match(TokenType::PUNC_RBRACE);

        return make_unique<node_types::WhileDecl>(std::move(while_exp), std::move(while_cmds));
    }
    else if (peek().type == TokenType::KW_SYSTEM) {
        match(TokenType::KW_SYSTEM);
        match(TokenType::PUNC_DOT);
        match(TokenType::KW_OUT);
        match(TokenType::PUNC_DOT);
        match(TokenType::KW_PRINTLN);
        match(TokenType::PUNC_LPARENT);
        auto print_exp = parse_Exp();
        match(TokenType::PUNC_RPARENT);
        match(TokenType::PUNC_SEMICOLON);

        return make_unique<node_types::PrintLn>(std::move(print_exp));
    }
    // left factoring applied here
    else {
        match(TokenType::IDENTIFIER);
        Token tkn_id = previous();

        Symbol* symb = env.lookup(tkn_id.lexeme);
        if(symb == nullptr){
            throw_error("Variável não declarada neste escopo: " + tkn_id.lexeme, ErrorPhase::SEMANTIC);
        }

        bool is_array = false;
        ExprNodePtr idx_exp = nullptr;

        if (peek().type == TokenType::PUNC_LBRACKET) {
            is_array = true;
            match(TokenType::PUNC_LBRACKET);
            idx_exp = parse_Exp();
            match(TokenType::PUNC_RBRACKET);
        }

        match(TokenType::OP_ASSIGN);
        auto rhs_exp = parse_Exp();
        match(TokenType::PUNC_SEMICOLON);

        return make_unique<node_types::AssignDecl>(tkn_id.lexeme, is_array, std::move(idx_exp), std::move(rhs_exp));
    }
}


ExprNodePtr Parser::parse_Exp() {
    parse_And_exp();
    return nullptr; 
}

ExprNodePtr Parser::parse_And_exp(){
    ExprNodePtr left = parse_Rel_exp();

    while(peek().type == TokenType::OP_AND){
        match(TokenType::OP_AND);
        ExprNodePtr right = parse_Rel_exp();
        left = make_unique<node_types::AndExpr>(std::move(left), std::move(right));
    }

    return left;
}

ExprNodePtr Parser::parse_Rel_exp(){
    ExprNodePtr left = parse_Add_exp();

    while(peek().type == TokenType::OP_GREATER){
        match(TokenType::OP_GREATER);
        ExprNodePtr right = parse_Add_exp();
        left = make_unique<node_types::RelExpr>(std::move(left), std::move(right));
    }

    return left;
}

ExprNodePtr Parser::parse_Add_exp(){
    ExprNodePtr left = parse_Mul_exp();
    node_types::AddExpr::Operation op;

    while(peek().type == TokenType::OP_PLUS || peek().type == TokenType::OP_MINUS){
        if(peek().type == TokenType::OP_PLUS){
            match(TokenType::OP_PLUS);
            op = node_types::AddExpr::Operation::SUM;
        }
        else{
            match(TokenType::OP_MINUS);
            op = node_types::AddExpr::Operation::SUB;
        }
        ExprNodePtr right = parse_Mul_exp();
        left = make_unique<node_types::AddExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ExprNodePtr Parser::parse_Mul_exp(){
    ExprNodePtr left = parse_Un_exp();

    while(peek().type == TokenType::OP_ASTERISK){
        match(TokenType::OP_ASTERISK);
        ExprNodePtr right = parse_Un_exp();
        left = make_unique<node_types::MulDivExpr>(node_types::MulDivExpr::Operation::MUL, std::move(left), std::move(right));
    }
    return left;
}

ExprNodePtr Parser::parse_Un_exp(){
    if(peek().type == TokenType::OP_NOT){
        match(TokenType::OP_NOT);
        ExprNodePtr un_exp_op = parse_Un_exp();

        return make_unique<node_types::NegateExpr>(true, std::move(un_exp_op));
    }
    else{
        return parse_Psf_exp();
    }
}

ExprNodePtr Parser::parse_Psf_exp(){
    ExprNodePtr left = parse_Pri_exp();

    while(peek().type == TokenType::PUNC_LBRACKET || peek().type == TokenType::PUNC_DOT){
        if(peek().type == TokenType::PUNC_LBRACKET){
            match(TokenType::PUNC_LBRACKET);
            vector<ExprNodePtr> access;
            ExprNodePtr idx = parse_Exp();
            access.push_back(std::move(idx));
            match(TokenType::PUNC_RBRACKET);
            left = make_unique<node_types::PrimaryAccessExpr>(std::move(left), node_types::PrimaryAccessExpr::PEModifier::ARRAY_ACCESS, std::move(access), vector<unique_ptr<ExprNode>>{});
        }
        else{
            match(TokenType::PUNC_DOT);
            //left factoring applied here
            if(peek().type == TokenType::KW_LENGTH){
                match(TokenType::KW_LENGTH);
                left = make_unique<node_types::PrimaryAccessExpr>(std::move(left), node_types::PrimaryAccessExpr::PEModifier::LENGTH, vector<unique_ptr<ExprNode>>{}, vector<unique_ptr<ExprNode>>{});
            }
            else{
                match(TokenType::IDENTIFIER);
                string method_id = previous().lexeme;
                match(TokenType::PUNC_LPARENT);
                std::vector<unique_ptr<ExprNode>> args = {};
                //handling zero argument function
                if(peek().type != TokenType::PUNC_RPARENT){
                    args = parse_ListExp();
                }

                match(TokenType::PUNC_RPARENT);
                left = make_unique<node_types::PrimaryAccessExpr>(std::move(left), node_types::PrimaryAccessExpr::PEModifier::METHOD_CALL, vector<unique_ptr<ExprNode>>{}, std::move(args));
            }
        }
    }
    return left;
}

ExprNodePtr Parser::parse_Pri_exp(){
    if(peek().type == TokenType::PUNC_LPARENT){
        match(TokenType::PUNC_LPARENT);
        ExprNodePtr exp = parse_Exp();
        match(TokenType::PUNC_RPARENT);
        return make_unique<node_types::PrimaryExpr>(std::move(exp));
    }
    else if(peek().type == TokenType::KW_TRUE){
        match(TokenType::KW_TRUE);
        return make_unique<node_types::TrueFalseLiteral>(true);
    }
    else if(peek().type == TokenType::KW_FALSE){
        match(TokenType::KW_FALSE);
        return make_unique<node_types::TrueFalseLiteral>(false);
    }
    else if(peek().type == TokenType::IDENTIFIER){
        Token tkn_id = peek();
        match(TokenType::IDENTIFIER);

        Symbol* symb = env.lookup(tkn_id.lexeme);
        if(symb == nullptr){
            throw_error("Variável não declarada neste escopo: " + tkn_id.lexeme, ErrorPhase::SEMANTIC);
        }

        return make_unique<node_types::IdLiteral>(tkn_id.lexeme);
    }
    else if(peek().type == TokenType::NUMBER_LITERAL){
        match(TokenType::NUMBER_LITERAL);
        return make_unique<node_types::NumLiteral>(std::stoll(previous().lexeme));
    }
    else if(peek().type == TokenType::KW_THIS){
        match(TokenType::KW_THIS);
        return make_unique<node_types::ThisExpr>();
    }
    else if(peek().type == TokenType::KW_NEW){
        match(TokenType::KW_NEW);
        if(peek().type == TokenType::KW_INT){
            match(TokenType::KW_INT);
            match(TokenType::PUNC_LBRACKET);
            ExprNodePtr size = parse_Exp();
            match(TokenType::PUNC_RBRACKET);
            return make_unique<node_types::NewArrayExpr>(std::move(size));
        }
        else{
            Token tkn_class = peek();
            match(TokenType::IDENTIFIER);
            match(TokenType::PUNC_LPARENT);
            match(TokenType::PUNC_RPARENT);

            Symbol* symb = env.lookup(tkn_class.lexeme);
            if(symb == nullptr || (*symb).kind != SymbolKind::CLASS){
                throw_error("Classe não declarada: " + tkn_class.lexeme, ErrorPhase::SEMANTIC);
            }

            return make_unique<node_types::NewObjExpr>(tkn_class.lexeme);
        }
    }
    else{
        throw_error("Esperava o começo de uma Exp.");
    }
    return nullptr;
}

vector<ExprNodePtr> Parser::parse_ListExp() {
    vector<ExprNodePtr> ret_vec;
    ret_vec.push_back(parse_Exp());
    while(peek().type == TokenType::PUNC_COMMA){
        match(TokenType::PUNC_COMMA);
        ret_vec.push_back(parse_Exp());
    }
    return ret_vec;
}
