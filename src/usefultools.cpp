#include "usefultools.h"
#include <cctype>
#include <cwctype>

#define ctus(c) static_cast<unsigned char>(c) /*char to unsigned char cast definition*/

void UFT::skip_ws(size_t &it, std::string &s){
    while(isspace(ctus(s[it])) && it < s.size() && s[it] != '\n'){
        it++;
    }
}

void UFT::bulk_add_alpha_num(size_t &it, std::string &input, std::string &output){
    while(isalnum(ctus(input[it]))){
        output+=input[it];
        it++;
    }
}

void UFT::bulk_add_punct(size_t &it, std::string &input, std::string &output){
    while(ispunct(ctus(input[it]))){
        output+=input[it];
        it++;
    }
}

bool UFT::safe_isalnum(char c){
    return isalnum(ctus(c)) != 0;
}
bool UFT::safe_ispunct(char c){
    return ispunct(ctus(c)) != 0;
}
bool UFT::safe_isspace(char c){
    return std::isspace(ctus(c)) != 0;
}
bool UFT::is_ws_relevant_char(char sc){
    std::string arith_ops = "+-/!&|<>";
    unsigned char c = ctus(sc);
    
    for(auto op: arith_ops){
        if(c==op){
            return true;
        }
    }
    return false;
}