#pragma once

#include <fstream>
#include "preprocessor.h"
#include "running_opts.h"

class Core{
private:
    std::fstream m_filestream;
    RunningOptions m_running_opts;
    Preprocessor m_preprocess;
public:
    Core(RunningOptions &opts){
        m_running_opts = opts;
        m_filestream.open(m_running_opts.input_file_path);
    }
    void run();
};