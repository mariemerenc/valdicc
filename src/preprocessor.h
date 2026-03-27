#pragma once
#include <fstream>
#include <string>
class Preprocessor{
private:
    static std::string transform_fstream_into_string(std::fstream &file);
public:
    static std::string preprocess_input(std::fstream &file);
};

