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

    /** @brief Possible states for the comment removal state machine */
    enum LexerState { NORMAL, LINE_COMMENT, BLOCK_COMMENT };
    LexerState current_state = NORMAL;

    //flag used to preserve structural indentation while allowing inline minification
    bool is_start_of_line = true;

    while(file_iterator < file.size()){
        // safely fetch current and next characters (lookahead) for symbol detection
        char current_char = file[file_iterator];
        char next_char = (file_iterator+1 < file.size() ? file[file_iterator+1] : '\0');

        // ------------------------------ [comment removal state machine] ------------------------------
        if(current_state == NORMAL){
            // check for the start of a single line comment "//"
            if(current_char == '/' && next_char == '/'){
                current_state = LINE_COMMENT;
                file_iterator += 2;
                continue;
            }
            // check for the start of a multi line comment "/*"
            else if(current_char == '/' && next_char == '*'){
                current_state = BLOCK_COMMENT;
                file_iterator += 2;
                continue;
            }
        }
        else if(current_state == LINE_COMMENT){
            // ignore characters until a newline is found
            if(current_char == '\n'){
                output_string += '\n'; // preserve newline to maintain accurate line tracking
                current_state = NORMAL;
                is_start_of_line = true;
            }
            file_iterator++;
            continue;
        }
        else if(current_state == BLOCK_COMMENT){
            // preserve all internal newlines within a block comment to maintain accurate line tracking
            if(current_char == '\n'){
                output_string += '\n';
            }
            if(current_char == '*' && next_char == '/'){
                current_state = NORMAL;
                file_iterator++;
            }
            file_iterator++;
            continue;
        }

        // ------------------------------ [minification and spacing logic] ------------------------------
        if(current_state == NORMAL){
            // 1. preserve structural newlines
            if(file[file_iterator] == '\n'){
                output_string += '\n';
                is_start_of_line = true;
                file_iterator++;

                prev_was_punct = false;
                prev_was_ws_semantic = false;
                continue;
            }

            // 2. handle whitespaces (indentation & inline spacing)
            if(UFT::safe_isspace(file[file_iterator])){
                if(is_start_of_line){
                    output_string += file[file_iterator];
                    file_iterator++;
                }
                else{
                    UFT::skip_ws(file_iterator, file);
                }
                continue;
            }

            //character found, the indentation phase is over
            is_start_of_line = false;

            // 3. handle letters and numbers
            if(UFT::safe_isalnum(file[file_iterator])){
                // prevent merging two distinct alphanumeric blocks (e.g. "int main")
                if(!prev_was_punct && changed){
                    output_string += " ";
                }
                UFT::bulk_add_alpha_num(file_iterator, file, output_string);

                prev_was_punct = false;
                prev_was_ws_semantic = false;
            }

            // 4. handle symbols and operators
            if(UFT::safe_ispunct(file[file_iterator])){
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
            changed = true;
        }
    }
    return output_string;
}
