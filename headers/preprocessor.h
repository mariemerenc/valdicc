#pragma once
#include <fstream>
#include <string>

/**
 * @brief Static utility class responsible for preprocessing the source code.
 * 
 * It reads the raw character stream from the input file and performs
 * minification (removing unnecessary whitespaces) and comment removal,
 * preparing a clean string for the lexical analysis phase.
 */
class Preprocessor{
private:
    /**
     * @brief Converts the contents of a file stream into a string.
     * 
     * Reads the entire buffer of an fstream object and stores it in main memory
     * as a single string to facilitate manipulation.
     * @param file Reference to the opened file stream.
     * @return std::string The full content of the file.
     */
    static std::string transform_fstream_into_string(std::fstream &file);
public:
    /**
     * @brief Performs the preprocessing of the source code.
     * 
     * Iterates through the read source code, eliminating unnecessary whitespaces
     * (minification) while preserving the logical separation of tokens based on
     * punctuation and alphanumeric characters.
     * 
     * TODO: Remover os comentários de linha e de bloco :)
     * @param file Reference to the input file stream.
     * @return std::string The preprocessed and minified source code.
     */
    static std::string preprocess_input(std::fstream &file);
};

