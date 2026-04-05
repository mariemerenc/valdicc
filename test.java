// TESTE 1: comentário de linha no começo do arquivo :D
class Abc {
  
  /* TESTE 2: gigantesco
  comentário
  de
  bloco
  ocupando
  várias
  linhas
  ebaaaaaaaaa */
  public static void main ( String [] args ) {
      
      // TESTE 3: espaços, tabs e quebras de linha
      int     variavel_1   =     10 ;
      int variavel_2=20;

      /* TESTE 4: comentário "dentro" de comentário ...? */
      // lalala /* comentário falso */ lalala
      /* lalala //comentário falso lalala */

      // TESTE 5: espaçamento semântico 
      // (+  + não pode virar ++)
      int x = 1 +  + 3 ; 
      
      // >  > não pode virar >>
      boolean y = a >  > b ; 
      
      /* TESTE 6: comentários dividindo comandos ! */
      int /* meio da declaracao */ z = 5;

      // TESTE 7: string com espaços !!
      String s = "ola mundo";

      // TESTE 8: garantia de que / não é confundido com comentário
      int w = a / b;

  }
}