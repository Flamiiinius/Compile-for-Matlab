%{
// HEADERS
#include <stdlib.h>
#include "parser.h"

// variables maintained by the lexical analyser
int yyline = 1;
%}

%option noyywrap

%%
[ \t]+ {  }
#.*\n { yyline++; }
\n { yyline++; }

\-?[0-9]+ {yylval.intValue = atoi(yytext); return INT;}


"==" { return EQ; }
"=" { return ATR; }
"if" { return IF; }
"*" { return MUL; }
"/" { return DIV; }
";" { return OO; }
"<=" { return LEEQ; }
">=" { return MOEQ; }
"+" { return PLUS; }
"~=" { return DIF; }
">" { return MORE; }
"-" { return MINUS; }
"<"  { return LESS; }
"true" { return TRUE; }
"false" { return FALSE; }
"for" { return FOR; }
"end" { return END; }
"elseif" { return ELSEIF; }
"else" { return ELSE; }
"while" { return WHILE; }
"disp" { return OUT; }
"input" { return IN; }
"(" { return OB; }
")" { return CB; }
":" { return COL; }

\-?[A-Z,a-z]+ {yylval.varval = strdup(yytext); return VAR;}

. { yyerror("unexpected character"); }
%%