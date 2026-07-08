#pragma once

#include <string>

/**
 * @brief Structure that holds the execution options of the compiler.
 * 
 * Stores the file paths and flags configured by the user via command-line
 * arguments, dictating how the Core application should behave.
 */
struct RunningOptions{
    /** @brief Path to the input source code file (e.g. a java file).*/
    std::string input_file_path = "";

    /** @brief Path to the output file where the preprocessed code will be saved.*/
    std::string clean_output_file_path = "";
    
    /** @brief Flag indicating whether the preprocessor output should be exported to a file.*/
    bool clean_output = false;

    bool symbtable_output = false;

    /** @brief Path to the file where the Lexer output will be saved.*/
    std::string lexer_output_file_path = "";
    
    /** @brief Flag indicating whether the Lexer output should be exported to a file.*/
    bool lexer_output = false;

    bool stop_on_first_error = false;

    bool suggest_corrections = false;

    std::string symbtable_output_file_path = "";

    /** @brief Path to the file where the AST tree printout will be saved.*/
    std::string ast_output_file_path = "";
 
    /** @brief Flag indicating whether the AST tree should be printed to a file.*/
    bool ast_output = false;
};