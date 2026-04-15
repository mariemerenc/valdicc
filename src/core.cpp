#include "core.h"
#include "preprocessor.h"
#include "lexer.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>

void Core::run(){
    std::string prep_out_string = Preprocessor::preprocess_input(m_filestream);
    if(m_running_opts.preprocessor_output){
        std::ofstream prep_outfile(m_running_opts.preprocessor_output_file_path);
        prep_outfile << prep_out_string;
        prep_outfile.close();
    }

    Lexer lexer(prep_out_string);
    std::vector<Token> tokens = lexer.tokenize();

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
}