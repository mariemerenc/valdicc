#pragma once

#include <string>


struct RunningOptions{
    std::string input_file_path = "";
    std::string preprocessor_output_file_path = "";
    bool preprocessor_output = false;
};