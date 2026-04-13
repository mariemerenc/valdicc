#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <unordered_map>
using namespace std;

/**
 * @brief Enumeration of all valid token types in the source language.
 * 
 * This enum categorizes all keywords, operators, punctuations, and 
 * dynamic literals recognized by the Lexer.
 */
enum class TokenType {
    //keywords
    KW_CLASS,       ///< 'class' keyword
    KW_PUBLIC,      ///< 'public' keyword
    KW_STATIC,      ///< 'static' keyword
    KW_VOID,        ///< 'void' keyword
    KW_MAIN,        ///< 'main' keyword
    KW_STRING,      ///< 'String' keyword
    KW_RETURN,      ///< 'return' keyword
    KW_INT,         ///< 'int' keyword
    KW_BOOLEAN,     ///< 'boolean' keyword
    KW_IF,          ///< 'if' keyword
    KW_ELSE,        ///< 'else' keyword
    KW_WHILE,       ///< 'while' keyword
    KW_SYSTEM,      ///< 'System' keyword
    KW_OUT,         ///< 'out' keyword
    KW_PRINTLN,     ///< 'println' keyword
    KW_LENGTH,      ///< 'length' keyword
    KW_NEW,         ///< 'new' keyword
    KW_TRUE,        ///< 'true' keyword
    KW_FALSE,       ///< 'false keyword
    KW_THIS,        ///< 'this' keyword

    //identifier or number literal
    IDENTIFIER,     ///< variable/method names
    NUMBER_LITERAL, ///< integer numeric values

    //operators
    OP_ASSIGN,      ///< '=' operator
    OP_AND,         ///< '&&' operator
    OP_GREATER,     ///< '>' operator
    OP_PLUS,        ///< '+' operator
    OP_MINUS,       ///< '-' operator
    OP_ASTERISK,    ///< '*' operator
    OP_NOT,         ///< '!' operator

    //punctuation
    PUNC_LBRACE,    ///< '{' left brace
    PUNC_RBRACE,    ///< '}' right brace
    PUNC_LPARENT,   ///< '(' left parenthesis
    PUNC_RPARENT,   ///< ')' right parenthesis
    PUNC_LBRACKET,  ///< '[' left bracket
    PUNC_RBRACKET,  ///< ']' right bracket
    PUNC_SEMICOLON, ///< ';' semicolon
    PUNC_COMMA,     ///< ',' comma
    PUNC_DOT,       ///< '.' dot

    //control
    ERROR_UNKNOWN,  ///< unrecognized sequence of characters
};


/**
 * @brief Represents a single lexical token extraced from the source code.
 * 
 * The Token struct encapsulates the categorized type of the lexeme, 
 * the exact string matched, and its location (line, column) 
 * to facilitate accurate syntax error reporting.
 */
struct Token {
    TokenType type; ///< The categorized type of the token
    string lexeme;  ///< The exact string matched from the source code
    int line;       ///< The line number where the token starts
    int column;     ///< The column number where the token starts
};


/**
 * @brief Dictionary mapping strings literals to keyword TokenTypes
 * 
 * Used by the Lexer to quickly verify if an extracted identifier is
 * actually a reserved keyword in the language (provides O(1) lookup time).
 */
const unordered_map<string, TokenType> KeywordMap = {
    {"class", TokenType::KW_CLASS},
    {"public", TokenType::KW_PUBLIC},
    {"static", TokenType::KW_STATIC},
    {"void", TokenType::KW_VOID},
    {"main", TokenType::KW_MAIN},
    {"String", TokenType::KW_STRING},
    {"return", TokenType::KW_RETURN},
    {"int", TokenType::KW_INT},
    {"boolean", TokenType::KW_BOOLEAN},
    {"if", TokenType::KW_IF},
    {"else", TokenType::KW_ELSE},
    {"while", TokenType::KW_WHILE},
    {"System", TokenType::KW_SYSTEM},
    {"out", TokenType::KW_OUT},
    {"println", TokenType::KW_PRINTLN},
    {"length", TokenType::KW_LENGTH},
    {"new", TokenType::KW_NEW},
    {"true", TokenType::KW_TRUE},
    {"false", TokenType::KW_FALSE},
    {"this", TokenType::KW_THIS}
};

#endif