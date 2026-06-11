#include "../headers/lexer.h"
#include "../headers/usefultools.h"
#include <regex>
#include <iostream>
#include <limits.h>
#include <stdexcept>
using namespace std;


TokenType Lexer::classify_lexeme(const string& lexeme){
    auto it = KeywordMap.find(lexeme);

    if(it != KeywordMap.end()){
        return it->second;
    }

    return TokenType::IDENTIFIER;
}

void Lexer::update_location(const string& string){
    for(char c : string){
        if(c == '\n'){
            current_line++;
            current_column=1;
        }
        else{
            current_column++;
        }
    }
}

void Lexer::handle_lexical_error(const string& invalid_lexeme){
    smatch matches;
    regex invalid_id_pattern("^[0-9]+[a-zA-Z][a-zA-Z0-9_]*");

    if(invalid_lexeme == " ") return;

    string full_msg = "[ERRO LÉXICO] na linha " +
                    to_string(current_line) +
                    ", coluna " +
                    to_string(current_column) + 
                    ":\n";

    if(regex_match(invalid_lexeme, matches, invalid_id_pattern)){
        full_msg += ("-> Identificador inválido: " +
                    invalid_lexeme + '\n' +
                    "-> REGRA: nomes de variáveis e métodos não podem começar com números!"
                    );

        if(m_running_opts.suggest_corrections){
            regex num_prefix("^[0-9]+");
            string suggestion = regex_replace(invalid_lexeme, num_prefix, "");

            full_msg += "\n-> Você quis dizer: '" + suggestion + "'?";
        }
    }
    else{
        full_msg += "-> Símbolo não reconhecido: " + invalid_lexeme;

        if(m_running_opts.suggest_corrections){
            string best_match;
            int best_dist = INT_MAX;

            for(const auto& [symbol, _] : SymbolMap){
                int dist = UFT::levenshtein_distance(invalid_lexeme, symbol);
                if(dist < best_dist){
                    best_dist = dist;
                    best_match = symbol;
                }
            }

            if(best_dist <= 1){
                full_msg += "\n-> Você quis dizer: '" + best_match + "'?";
            }
            full_msg += '\n';
        }
    }
    if(m_running_opts.stop_on_first_error){
        throw runtime_error(full_msg);
    }
    else{
        lexical_errors.push_back(full_msg);
    }
}

vector<Token> Lexer::tokenize(){
    vector<Token> tokens;
    /**
     * @brief Regular expression that captures all valid lexemes.
     * 
     * 1st group: matches line comments and block comments;
     * 
     * 2nd group: \s matches any whitespace character, + matches preceding token >= 1 times;
     * 
     * 3rd group: [a-zA-Z] match a single alphabetic character, [a-zA-Z0-9_] match alphanumeric characters (allowing underscore occurrences), * matches preceding token >= 0 times;
     * 
     * 4th group: ([0-9]+[a-zA-Z_][a-zA-Z0-9_]*) matches invalid identifiers;
     * 
     * 5th group: [0-9] match a single numeric character, + matches preceding token >= 1 times;
     * 
     * 6th group: && matches the characters && literally, [=+\-!><;\*,.\[\]{}()] match ...
     */
    static const regex token_regex(R"((//[^\n]*|/\*[\s\S]*?\*/)|(\s+)|([a-zA-Z_][a-zA-Z0-9_]*)|([0-9]+[a-zA-Z_][a-zA-Z0-9_]*)|([0-9]+)|(&&|[=+\-!><;\*,.\[\]{}()]))");

    size_t expected_pos = 0;
    auto it_begin = sregex_iterator(source_code.begin(), source_code.end(), token_regex);
    auto it_end = sregex_iterator();

    for(auto i = it_begin; i != it_end; i++){
        smatch match = *i;
        string lexeme = match.str();
        size_t match_pos = match.position();

        if(match_pos > expected_pos){
            string invalid_txt = source_code.substr(expected_pos, match_pos - expected_pos);
            handle_lexical_error(invalid_txt);
        }

        int start_line = current_line;
        int start_column = current_column;

        update_location(lexeme);

        expected_pos = match_pos + lexeme.length();

        if(match[1].matched){
            for(char l : lexeme){
                if(l == '\n'){
                    clean_source_code += '\n';
                }
            }
        }
        else{
            clean_source_code += lexeme;
        }

        // 1st and 2nd cases: comments and whitespaces
        if(match[1].matched || match[2].matched){
            continue;
        }
        // 3rd case: identifier or keyword
        else if(match[3].matched){
            TokenType tkn_type = classify_lexeme(lexeme);
            tokens.push_back({tkn_type, lexeme, start_line, start_column});
        }
        //4th case: invalid identifier
        else if(match[4].matched){
            handle_lexical_error(lexeme);
        }
        // 5th case: number
        else if(match[5].matched){
            tokens.push_back({TokenType::NUMBER_LITERAL, lexeme, start_line, start_column});
        }
        // 6th case: punctuation or operator
        else if(match[6].matched){
            auto it = SymbolMap.find(lexeme);
            if(it != SymbolMap.end()){
                tokens.push_back({it->second, lexeme, start_line, start_column});
            }
            else{
                handle_lexical_error(lexeme);
            }
        }
    }

    if(expected_pos < source_code.length()){
        string invalid_text = source_code.substr(expected_pos);
        handle_lexical_error(invalid_text);
    }

    if(!lexical_errors.empty()){
        string all_errors = "";

        for(const string& err : lexical_errors){
            all_errors += err + '\n';
        }
        throw runtime_error(all_errors);
    }

    return tokens;
}