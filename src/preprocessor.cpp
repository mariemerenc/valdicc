#include "preprocessor.h"
#include "usefultools.h"
#include <cctype>
#include <cwctype>
#include <fstream>
#include <iostream>
#include <sstream>


std::string Preprocessor::transform_fstream_into_string(std::fstream &file){
    std::stringstream s;
    s << file.rdbuf();
    return s.str();
}

std::string Preprocessor::preprocess_input(std::fstream &filestream){
    std::string output_string = "";
    std::string file = transform_fstream_into_string(filestream);
    
    size_t file_iterator = 0;

    //state flags to preserve semantic spacing and prevent token agglutination
    bool prev_was_punct = false;
    bool prev_was_ws_semantic=false;
    bool changed = false;

    UFT::skip_ws(file_iterator, file);

    while(file_iterator < file.size()){
        //--------------------------------------------------
        // TODO: remoção de comentários de linha e de bloco.
        //--------------------------------------------------

        // 1. handle letters and numbers
        if(UFT::safe_isalnum(file[file_iterator])){
            // prevent merging two distinct alphanumeric blocks (e.g. "int main")
            if(!prev_was_punct && changed){
                output_string += " ";
            }
            UFT::bulk_add_alpha_num(file_iterator, file, output_string);

            prev_was_punct = false;
            prev_was_ws_semantic = false;
        }

        // 2. handle symbols and operators
        else if(UFT::safe_ispunct(file[file_iterator])){
            // add space if the previous char was a semantic sensitive operator
            if(prev_was_ws_semantic){
                output_string += " ";
            }
            UFT::bulk_add_punct(file_iterator, file, output_string);

            // check if the last consumed punctuation requires a following space
            if(UFT::is_ws_relevant_char(file[file_iterator-1])){
                prev_was_ws_semantic = true;
            }
            else{
                prev_was_ws_semantic = false;
            }
            prev_was_punct = true;
        }

        // 3. minification: ignore unneeded whitespaces and newlines
        else if(UFT::safe_isspace(file[file_iterator])){
            UFT::skip_ws(file_iterator, file);
        }
        changed = true;
    }
    return output_string;
}
