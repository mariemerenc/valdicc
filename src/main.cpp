#include "../headers/core.h"
#include "../headers/running_opts.h"
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
    std::string clean_output_filename = "";
    std::string lexer_output_filename = "";
    std::string symbtable_output_filename = "";
    std::string ast_output_filename = "";

    cli_app.add_option("input", input_filename, ".java input file")
    ->required()
    ->check(CLI::ExistingFile);

    cli_app.add_option("-c", clean_output_filename, "Clean (''preprocessed'') output");
    cli_app.add_option("-l", lexer_output_filename, "Lexer output");
    cli_app.add_flag("--stop-on-first-error", run_opts.stop_on_first_error, "");
    cli_app.add_flag("--suggest", run_opts.suggest_corrections, "");
    cli_app.add_option("-p", symbtable_output_filename, "");
    cli_app.add_option("-t", ast_output_filename, "AST tree output");

    try{cli_app.parse(argc, argv);}
    catch(CLI::ParseError){
        std::cerr << "parse error\n";
        return 1;
    }
    
    run_opts.input_file_path = input_filename;
    if(clean_output_filename != ""){
        run_opts.clean_output_file_path = clean_output_filename;
        run_opts.clean_output = true;
    }
    if(lexer_output_filename != ""){
        run_opts.lexer_output_file_path = lexer_output_filename;
        run_opts.lexer_output = true;
    }
    if(symbtable_output_filename != ""){
        run_opts.symbtable_output_file_path = symbtable_output_filename;
        run_opts.symbtable_output = true;
    }
    if(ast_output_filename != ""){
        run_opts.ast_output_file_path = ast_output_filename;
        run_opts.ast_output = true;
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
        std::cerr << "Invalid arguments. Try running\n./build/vcc tests/\'input_file\'.ling -c \'clean_output_file\'.txt -l \'token_list_output\'.txt -p \'symbol_table_output\'.txt -t \'ast_output\'.txt\n";
        return 1;
    }
    Core core_app(run_opts);
    core_app.run();
}