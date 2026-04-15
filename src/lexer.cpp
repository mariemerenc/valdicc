#include "lexer.h"
#include <regex>
#include <iostream>
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

    cout << "[ERRO LÉXICO] na linha " << current_line << ", coluna " << current_column << ":\n";

    if(regex_match(invalid_lexeme, matches, invalid_id_pattern)){
        cout << "-> Identificador inválido: " << invalid_lexeme << '\n';
        cout << "-> REGRA: nomes de variáveis e métodos não podem começar com números!";
    }
    else{
        cout << "-> Símbolo não reconhecido: " << invalid_lexeme << '\n';
    }
}

vector<Token> Lexer::tokenize(){
    vector<Token> tokens;
    /**
     * @brief Regular expression that captures all valid lexemes.
     * 
     * 1st group: \s matches any whitespace character, + matches preceding token >= 1 times;
     * 
     * 2nd group: [a-zA-Z] match a single alphabetic character, [a-zA-Z0-9_] match alphanumeric characters (allowing underscore occurrences), * matches preceding token >= 0 times;
     * 
     * 3rd group: [0-9] match a single numeric character, + matches preceding token >= 1 times;
     * 
     * 4th group: && matches the characters && literally, [=+\-!><;\*,.\[\]{}()] match ...
     */
    static const regex token_regex(R"((\s+)|([a-zA-Z_][a-zA-Z0-9_]*)|([0-9]+)|(&&|[=+\-!><;\*,.\[\]{}()]))");

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

        // 1st case: whitespaces
        if(match[1].matched){
            continue;
        }
        // 2nd case: identifier or keyword
        else if(match[2].matched){
            TokenType tkn_type = classify_lexeme(lexeme);
            tokens.push_back({tkn_type, lexeme, start_line, start_column});
        }
        // 3rd case: number
        else if(match[3].matched){
            tokens.push_back({TokenType::NUMBER_LITERAL, lexeme, start_line, start_column});
        }
        // 4th case: punctuation or operator
        else if(match[4].matched){
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

    return tokens;
}