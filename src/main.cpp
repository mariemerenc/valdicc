#include "core.h"
#include <iostream>
#include <CLI/CLI.hpp>

/**
 * @brief Parses command-line arguments.
 * 
 * Uses the CLI11 library to configure and validate the options passed by
 * the user in the terminal, setting the required input file and optional output file.
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @param cli_app Reference to the CLI11 application object. 
 * @param run_opts Structure where the validated options will be stored.
 * @return int Returns 0 on success, or 1 in case of a parsing error.
 */
int parse_argv(int argc, char** argv, CLI::App &cli_app, RunningOptions &run_opts){
    std::string input_filename = "";
    std::string preprocessor_output_filename = "";
    std::string lexer_output_filename = "";

    cli_app.add_option("input", input_filename, ".java input file")
    ->required()
    ->check(CLI::ExistingFile);

    cli_app.add_option("-p", preprocessor_output_filename, "Preprocessor output");
    cli_app.add_option("-l", lexer_output_filename, "Lexer output");
    
    try{cli_app.parse(argc, argv);}
    catch(CLI::ParseError){
        std::cerr << "parse error\n";
        return 1;
    }
    
    run_opts.input_file_path = input_filename;
    if(preprocessor_output_filename != ""){
        run_opts.preprocessor_output_file_path = preprocessor_output_filename;
        run_opts.preprocessor_output = true;
    }
    if(lexer_output_filename != ""){
        run_opts.lexer_output_file_path = lexer_output_filename;
        run_opts.lexer_output = true;
    }
    
    
    return 0;
}

/**
 * @brief Entry point for the VCC compiler
 * 
 * Initializes the configuration structures, parses the command-line arguments,
 * and delegates execution to the Core class.
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return int Returns 0 upon successfull execution, or 1 in case of errors.
 */
int main(int argc, char** argv){
    RunningOptions run_opts = RunningOptions();
    CLI::App cli_app("Compiler commandline parser", "vcc");
    if(parse_argv(argc, argv, cli_app, run_opts) != 0){
        std::cerr << "Invalid arguments. Try running\nvcc \'input_file\'.java -p \'output_file\'.txt\n";
        return 1;
    }
    Core core_app(run_opts);
    core_app.run();
}