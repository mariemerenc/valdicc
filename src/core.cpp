#include "core.h"
#include <fstream>

void Core::run(){
    std::string prep_out_string = Preprocessor::preprocess_input(m_filestream);
    if(m_running_opts.preprocessor_output){
        std::ofstream prep_outfile(m_running_opts.preprocessor_output_file_path);
        prep_outfile << prep_out_string;
        prep_outfile.close();
    }
}