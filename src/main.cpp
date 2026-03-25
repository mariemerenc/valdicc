#include "CLI/App.hpp"
#include <iostream>
#include <CLI/CLI.hpp>

int main(int argc, char** argv){
    CLI::App cli_app("parse");
    bool o = false;
    cli_app.add_flag("-o", o, "output");
    CLI11_PARSE(cli_app, argc, argv);
    std::cout << o << "\n";
}