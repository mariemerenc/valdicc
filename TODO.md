TODO:
- formalizar a gramatica refatorada
- aplicar first and follow na gramática
- aplicar prioridades PEMDAS na gramatica
- codar de fato as produções no parser
- tabela de simbolos como tabelas hash empilhadas
- perguntar a valdis sobre a tabela de simbolos e a questao da pilha de escopos vs so uma hash com uma categoria de escopo
            **** also a ideia eh colocar um ponteiro para a entrada da symtab no token eh??? se sim, precisamos refatorar essa parte



notas de carol:
- top down
- recursive predictive parsing
- ela nao pode ser nem left recursive nem ambigua
- WTF IS PANIC MODE WHAT
- dificuldades encontradas: lambda productions dificultam a producao do codigo; so deixar nao retornar nada nao funciona pois - > se temos uma producao que deriva ou outra producao ou lambda, nao sabemos o que estamos derivando!! temos ambiguidades!!!!!


ordem da gramatica:

1 - new id (), parentese
2 - length
3 - listexp
4 - acesso []
5 - *
6 - +-


diferente:
1 - ()
2 - !, >
3 - &&




ultimos: 
