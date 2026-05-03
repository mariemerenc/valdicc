#pragma once
#include <string>

/**
 * @brief Useful Tools namespace containing helper functions for character and string manipulation.
 * 
 * This namespace groups standalone utility functions that safely check character 
 * types and handle the bulk reading of alphanumeric and punctuation characters.
 */
namespace UFT{
    /**
     * @brief Determines if an adjacent whitespace is semantically relevant.
     * 
     * Checks if the previous character is an operator that requires special
     * attention regarding spaces to prevent improper token agglutination
     * (e.g. two '+' signs becoming an increment '++').
     * @param sc Character to be analyzed.
     * @return true If the character is a restricted arithmetic or logical operator,
     * @return false otherwise.
     */
    bool is_ws_relevant_char(char sc);
    /**
     * @brief Safely checks if a character is alphanumeric.
     * @param c Character to be checked.
     * @return true If it is a letter or a number,
     * @return false otherwise.
     */
    bool safe_isalnum(char c);
    /**
     * @brief Safely checks if a character is punctuation.
     * @param c Character to be checked.
     * @return true If it is punctuation,
     * @return false otherwise.
     */
    bool safe_ispunct(char c);
    /**
     * @brief Safely checks if a character is whitespace.
     * @param c Character to be checked.
     * @return true If it is a space, tab, or newline,
     * @return false otherwise.
     */
    bool safe_isspace(char c);
    /**
     * @brief Advances the iterator, skipping whitespace characters.
     * @param it Reference to the current iterator (string index); 
     * @param s String containing the source code being analyzed. 
     */
    void skip_ws(size_t &it, std::string &s);
    /**
     * @brief Sequentially collects alphanumeric characters.
     * 
     * Reads characters from the input that form identifiers or numbers
     * and appends them to the output string until a different character type is found.
     * @param it Reference to the current iterator (string index).
     * @param input String containing the original source code.
     * @param output String where the minified result is being built. 
     */
    void bulk_add_alpha_num(size_t &it, std::string &input, std::string &output);
    /**
     * @brief Sequentially collects punctuation characters.
     * 
     * Reads graphical signs and mathematical/logical operators from the input
     * and appends them to the output string.
     * @param it Reference to the current iterator (string index).
     * @param input String containing the original source code. 
     * @param output String where the minified result is being built. 
     */
    void bulk_add_punct(size_t &it, std::string &input, std::string &output);
}
