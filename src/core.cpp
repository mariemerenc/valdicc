#include "../headers/core.h"
#include "../headers/preprocessor.h"
#include "../headers/lexer.h"
#include "../headers/parser.h"
#include "../headers/symbol_table.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>

void Core::run(){
    std::stringstream s;
    s << m_filestream.rdbuf();
    std::string source_code = s.str();

    Lexer lexer(source_code, m_running_opts);

    std::vector<Token> tokens;

    try{
        tokens = lexer.tokenize();
    }
    catch(const exception& e){
        cerr << e.what() << '\n';
        return;
    }

    if(m_running_opts.clean_output){
        std::ofstream clean_outfile(m_running_opts.clean_output_file_path);
        clean_outfile << lexer.clean_source_code;
        clean_outfile.close();
    }

    if(m_running_opts.lexer_output){
        std::ofstream lexer_outfile{m_running_opts.lexer_output_file_path};

        if(lexer_outfile.is_open()){
            for(const auto& token : tokens){
                lexer_outfile << std::left << std::setw(15) << to_string(token.type) 
                            << " | Lexeme: " << std::setw(10) << token.lexeme 
                            << " | Line: " << token.line 
                            << " | Column: " << token.column << "\n";
            }
            lexer_outfile.close();
        }
    }

    SymbolTable table; //ta vazia
    Parser parser(tokens, table, m_running_opts);

    try{
        parser.parse();
    }
    catch(const exception& e){
        cerr << "[ERRO SINTÁTICO] " << e.what() << '\n';
    }
}