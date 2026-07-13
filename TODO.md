# TODO DA IMPLEMENTAÇÃO DA ÁRVORE SINTÁTICA

## 1. Precisa terminar de refatorar o parse 

### A. Finalizar o parse command
### B. Finalizar o parse das expressões

## 2. Merge com as mudanças da tabela de símbolos

## 3. Precisa realizar a visita na árvore para fazer a análise estática de tipo e de valores.

## 4. Precisa fazer o print da AST sobrecarregando o método to_string de todos os nós




## Precisa
## TODO:
- formalizar a gramatica refatorada
- aplicar first and follow na gramática
- aplicar prioridades PEMDAS na gramatica
- codar de fato as produções no parser
- tabela de simbolos como tabelas hash empilhadas
- perguntar a valdis sobre a tabela de simbolos e a questao da pilha de escopos vs so uma hash com uma categoria de escopo
            **** also a ideia eh colocar um ponteiro para a entrada da symtab no token eh??? se sim, precisamos refatorar essa parte



main generate function:

(partially) implemented

ASSIGNDECL,
IFELSEDECL,
WHILEDECL,
PRINTLN,
ANDEXPR,
RELEXPR,
ADDEXPR,
MULDIVEXPR,
NEGATEEXPR,
TRUEFALSELITERAL,
NUMLITERAL,
IDLITERAL,
VARDECL, (lol isso aqui so nao exploramos porq nao precisa)


to be implemented:
PROG,
MAINDECL,
CLASSDECL,
METHODDECL,
COMMANDDECL,
PRIMARYACCESSEXPR,
PRIMARYEXPR,
THISEXPR,
NEWOBJEXPR,
NEWARRAYEXPR
