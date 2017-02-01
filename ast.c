#include <stdlib.h> // for malloc
#include "ast.h" // AST header
#include <string.h>

List* ast_list(Com* cmds, List* nextt) { //LIST
  List* node = (List*) malloc(sizeof(List));
  node->command = cmds;
  node->next=nextt;
  return node;
}

Expr* ast_integer(int v){ //EXPRESSION -> INTEGER
  Expr* node = (Expr*) malloc(sizeof(Expr));
  node->kind = E_INTEGER;
  node->attr.value = v;
  return node;
}

Expr* ast_operation(int operator, Expr* left, Expr* right) {  //EXPRESSION -> OPERATION
  Expr* node = (Expr*) malloc(sizeof(Expr));
  node->kind = E_OPERATION;
  node->attr.op.operator = operator;
  node->attr.op.left = left;
  node->attr.op.right = right;
  return node;
}

Expr* ast_var(char* var){ //EXPRESSION -> VARIABEL
  Expr* node =(Expr*) malloc(sizeof(Expr));
  node->kind = E_VAR;
  node->attr.name = strdup(var);
  return node;
}

Com* ast_atrib(char* var, Expr* e){  //COMMAND -> ATRRIBUTION
  Com* node = (Com*) malloc(sizeof(Com));
  node->kind = C_ATR;
  node->attr.atrib.var=strdup(var);
  node->attr.atrib.expr=e;
  return node;
}

Com* ast_if(Expr* expr, List* next){
  Com* node= (Com*) malloc(sizeof(Com));
  node->kind=C_IF;
  node->attr.iff.expr=expr;
  node->attr.iff.next=next;
  return node;
} 

Com* ast_ifs(Expr* expr, List* next, Ifs* nextcondition){
  Com* node= (Com*) malloc(sizeof(Com));
  node->kind=C_IFS;
  node->attr.ifs.expr=expr;
  node->attr.ifs.next=next;
  node->attr.ifs.nextcondition=nextcondition;
  return node;
} 

Ifs* ast_elseif(Expr* expr, List* next ,Ifs* nextcondition){
  Ifs* node= (Ifs*) malloc(sizeof(Ifs));
  node->kind=IF_ELSEIF;
  node->attr.elseifs.expr=expr;
  node->attr.elseifs.next=next;
  node->attr.elseifs.nextcondition=nextcondition;
  return node;
} 

Ifs* ast_else(List* next){
  Ifs* node= (Ifs*) malloc(sizeof(Ifs));
  node->kind=IF_ELSE;
  node->attr.elses.next=next;
  return node;
} 

Ifs* ast_elif(Expr* expr, List* next){
  Ifs* node= (Ifs*) malloc(sizeof(Ifs));
  node->kind=IF_ELIF;
  node->attr.elif.expr=expr;
  node->attr.elif.next=next;
  return node;
} 

Com* ast_while(Expr* expr, List* next){
  Com* node= (Com*) malloc(sizeof(Com));
  node->kind=C_WHILE;
  node->attr.whiles.expr=expr;
  node->attr.whiles.next=next;
  return node;
} 

Com* ast_for(char* var, int i,Expr* expr, List* next){
  Com* node= (Com*) malloc(sizeof(Com));
  node->kind=C_FOR;
  node->attr.fors.var=var;
  node->attr.fors.i=i;
  node->attr.fors.expr=expr;
  node->attr.fors.next=next;
  return node;
} 

Com* ast_out(Expr* expr){
  Com* node = (Com*) malloc(sizeof(Com));
  node->kind=C_OUT;
  node->attr.out.expr=expr;
  return node;
}

Com* ast_in(char* name){
  Com* node = (Com*) malloc(sizeof(Com));
  node->kind=C_IN;
  node->attr.in.name=name;
  return node;
}