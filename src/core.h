#pragma once

#include <fstream>
#include "preprocessor.h"
#include "running_opts.h"

/**
 * @brief Constructs the Core application.
 * 
 * Initializes the main application by storing the execution options
 * and opening the input file stream specified by the user.
 * * @param opts Reference to the running options (RunningOptions) parsed from the command line.
 */
class Core{
private:
    /** @brief The input file stream used to read the source code. */
    std::fstream m_filestream;

    /** @brief Configuration settings and flags for the current execution session.*/
    RunningOptions m_running_opts;

    /** @brief Instance of the Preprocessor used to clean and minify the input stream. */
    Preprocessor m_preprocess;
public:
    Core(RunningOptions &opts){
        m_running_opts = opts;
        m_filestream.open(m_running_opts.input_file_path);
    }
    /**
     * @brief Executes the mains preprocessor flow.
     * 
     * Reads the input file, calls the preprocessing module to perform minification
     * and comment removal, and, if requested in the running options, writes the
     * resulting code to an output file.
     */
    void run();
};