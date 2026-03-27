#pragma once
#include <string>

namespace UFT{
    bool is_ws_relevant_char(char c);
    bool safe_isalnum(char c);
    bool safe_ispunct(char c);
    bool safe_isspace(char c);
    void skip_ws(size_t &it, std::string &s);
    void bulk_add_alpha_num(size_t &it, std::string &input, std::string &output);
    void bulk_add_punct(size_t &it, std::string &input, std::string &output);
}
