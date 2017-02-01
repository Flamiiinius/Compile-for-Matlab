%token 
  INT
  VAR
  ATR
  PLUS
  MINUS
  DIV
  MUL
  MOD
  MORE
  EQ
  DIF
  LESS
  LEEQ
  MOEQ
  FOR
  IF
  THEN
  ELSE
  WHILE
  END
  OUT
  IN
  OB
  CB
  COL
  ELSEIF
  OO
  TRUE
  FALSE

%left EQ DIF LESS MORE LEEQ MOEQ
%left PLUS MINUS
%left MUL DIV
%left OB 

%start program;

%union {
  int intValue;
  char* varval;
  List* list;
  Expr* exprValue; 
  Com* command;
  Ifs* iff;
}

%type <intValue> INT
%type <varval> VAR
%type <list> cmdlist
%type <exprValue> expr
%type <command> cmdss
%type <iff> elist

%code requires {
  #include <stdio.h>
  #include <stdlib.h>
  #include "ast.h"

  extern int yylex();
  extern int yyline;
  extern char* yytext;
  extern FILE* yyin;
  extern void yyerror(const char* msg);
  struct CommandList* root;
}

%%
program: cmdlist { root = $1; }

cmdlist: cmdss {$$ = ast_list($1,NULL);}
       | cmdss cmdlist {$$ = ast_list($1,$2);}

cmdss: VAR ATR expr OO {$$ = ast_atrib($1,$3);}
      | IF expr cmdlist END {$$ = ast_if($2,$3);}
      | IF expr cmdlist elist {$$ = ast_ifs($2,$3,$4);}
      | WHILE expr cmdlist END {$$ = ast_while($2,$3);}
      | FOR VAR ATR INT COL expr cmdlist END {$$ = ast_for($2,$4,$6,$7);}
      | OUT OB expr CB OO {$$ = ast_out($3);}
      | VAR ATR IN OB CB OO {$$ = ast_in($1);}

elist: ELSEIF expr cmdlist elist {$$ = ast_elseif($2,$3,$4);}
      | ELSEIF expr cmdlist END {$$ = ast_elif($2,$3);}
      | ELSE cmdlist END {$$ = ast_else($2);}

expr: INT {$$ = ast_integer($1);}
    | VAR {$$ = ast_var($1);}
    | OB expr CB {$$ = $2;}
    | TRUE {$$ = ast_integer(1); }
    | FALSE { $$ = ast_integer(0); }
    | expr PLUS expr {$$ = ast_operation(PLUS, $1, $3);}
    | expr MINUS expr {$$ = ast_operation(MINUS, $1, $3);}
    | expr MUL expr {$$ = ast_operation(MUL, $1, $3);}
    | expr DIV expr {$$ = ast_operation(DIV, $1, $3);}
    | expr EQ expr {$$ = ast_operation(EQ, $1, $3);}
    | expr DIF expr {$$ = ast_operation(DIF, $1, $3);}
    | expr MORE expr {$$ = ast_operation(MORE, $1, $3);}
    | expr LESS expr {$$ = ast_operation(LESS, $1, $3);}
    | expr MOEQ expr {$$ = ast_operation(MOEQ, $1, $3);}
    | expr LEEQ expr {$$ = ast_operation(LEEQ, $1, $3);}
    ;

%%

void yyerror(const char* err) {printf("Line %d: %s - '%s'\n", yyline, err, yytext  );}