# Valdi Compact Compiler - VCC

This is the compiler project for the class **DIM0164 - Compilers**. 


## Features

Currently, the project acts as a preprocessor and includes the following functionalities:
* Source code minification (removal of unnecessary whitespaces and newlines).
* Preservation of token separation (prevents agglutination).
* Removal of line ('//') and block ('/* ... */') comments.

## Prerequisites

To build and run this project, you will need:
* CMake (version 3.16 or higher)
* A C++20 compatible compiler (e.g. GCC or CLang)
* Doxygen (for generating documentation)
## How to build

This is a C++20 project. To configure and compile, run the following commands in the root directory:
```bash 
cmake -S . -B build
cmake --build build
```
## How to run

To run the file preprocessor for .java files, run:
```bash 
./build/vcc <input_file>.java -p <output_file>.txt
```
## Documentation

This project uses **Doxygen** to automatically generate technical documentation (classes, methods and namespaces) from the source code comments.

Because the generated HTML files are not versioned in this repository, you can generate the documentation locally by following these steps:

### 1. Install Doxygen (on Debian/Ubuntu-based Linux distributions)
```bash
sudo apt update
sudo apt install doxygen
```

### 2. Generate the documentation

At the root of the project (where the Doxyfile is located), execute:
```bash
doxygen Doxyfile
```

### 3. View the result
To view the documentation, open the following file in your preferred web browser:
```plaintext
docs/html/index.html
```
## Authors

- [Arthur José Arruda Skeete](https://github.com/ArthurJoseAS)
- [Carolina Nunes de Carvalho](https://github.com/carolln)
- [Mariana Emerenciano Miranda](https://github.com/mariemerenc)
- [Marina Medeiros de Araújo Leite](https://github.com/marina-medeiros)

