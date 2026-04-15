#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <regex>
#include "token.h"

/**
 * @class Lexer
 * @brief Performs lexical analysis on preprocessed source code.
 * 
 * The Lexer scans the input string using regex to identify 
 * valid tokens, tracking their line and column positions.
 */
class Lexer{
    private:
    /** @brief The preprocessed source code string to be tokenized.*/
    std::string source_code; 
    /** @brief Current line number (starts at 1).*/
    int current_line;
    /** @brief Current column number (starts at 1).*/
    int current_column;

    /**
     * @brief Determines if a lexeme is a keyword or an identifier.
     * 
     * @param lexeme The string extracted from the source code.
     * @return TokenType The specific keyword type or TokenType::IDENTIFIER.
     */
    TokenType classify_lexeme(const std::string& lexeme);

    /**
     * @brief Advances line and column counters based on the characters in the provided string.
     * 
     * @param string The string just consumed.
     */
    void update_location(const std::string& string);

    /**
     * @brief Processes unrecognized character sequences and reports lexical errors.
     * 
     * @param invalid_lexeme Invalid sequence of characters.
     */
    void handle_lexical_error(const std::string& invalid_lexeme);

    public:
    /** @brief Lexer constructor.*/
    Lexer(const std::string& preprocessor_output) {
        current_column = 1; 
        current_line = 1;
        source_code = preprocessor_output;
    };

    /**
     * @brief Main method that scans the source code and generates a stream of tokens.
     * 
     * Uses regex to identify whitespaces, identifiers, numbers, and symbols.
     * 
     * @return std::vector<Token> A list of all identified tokens.
     */
    std::vector<Token> tokenize();

};

#endif