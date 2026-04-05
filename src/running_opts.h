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
    std::string preprocessor_output_file_path = "";
    
    /** @brief Flag indicating whether the preprocessor output should be exported to a file.*/
    bool preprocessor_output = false;
};