#include "../headers/usefultools.h"
#include <vector>
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
int UFT::levenshtein_distance(const std::string& a, const std::string& b){
    int size_a = a.size();
    int size_b = b.size();
    std::vector<std::vector<int>> verif(size_a + 1, std::vector<int>(size_b + 1));

    if(size_a == 0)
        return size_b;

    if(size_b == 0)
        return size_a;

    for (int i = 0; i <= size_a; i++)
        verif[i][0] = i;
    for (int j = 0; j <= size_b; j++)
        verif[0][j] = j;

    for(int ii = 1; ii <= size_a; ii++){
        for(int jj = 1; jj <= size_b; jj++){
            int substitionCost;
            if(a[ii-1] == b[jj-1]){
                substitionCost = 0;
            }else{
                substitionCost = 1;
            }

            verif[ii][jj] = std::min(std::min(verif[ii-1][jj] + 1, 
                                              verif[ii][jj-1] + 1), 
                                    verif[ii-1][jj-1] + substitionCost);
        }
    }
    
    return verif[size_a][size_b];
}