#ifndef __ast_h__
#define __ast_h__

struct Expression {
  enum {E_INTEGER, E_OPERATION, E_VAR} kind;
  union {
    int value; // for integer values
    char* name; //for a variable
    struct {int operator;struct Expression* left;struct  Expression* right;} op; // PLUS MINUS MOD MULT DIV | EQ DIF MORE LESS MOEQ LEEQ
  } attr;
};

struct Command{
  enum{C_ATR, C_IF, C_IFS, C_WHILE, C_FOR, C_IN, C_OUT} kind;
  union{
    struct{char* var; struct Expression* expr;} atrib;
    struct{struct Expression* expr; struct CommandList* next;} iff;
    struct{struct Expression* expr; struct CommandList* next; struct IfList* nextcondition;} ifs;
    struct{struct Expression* expr; struct CommandList* next;} whiles;
    struct{char* var; int i; struct Expression* expr; struct CommandList* next;} fors;
    struct{struct Expression* expr;} out;
    struct{char* name;} in;
  }attr;
};

struct IfList{
  enum{IF_ELSEIF, IF_ELIF,IF_ELSE} kind;
  union{
    struct {struct Expression* expr; struct CommandList* next; struct IfList* nextcondition;} elseifs;
    struct {struct Expression* expr; struct CommandList* next;} elif;
    struct {struct CommandList* next;} elses;
  } attr;
};

struct CommandList{
  struct Command* command;
  struct CommandList* next;
};

typedef struct Expression Expr;
typedef struct Command Com;
typedef struct CommandList List;
typedef struct IfList Ifs;

Expr* ast_integer(int v);
Expr* ast_operation(int operator,Expr* left,Expr* right);
Expr* ast_var(char* var);
Com* ast_while(Expr* expr, List* next);
Com* ast_for(char* var, int i, Expr* expr, List* next);
Com* ast_out(Expr* expr);
Com* ast_in(char* name);
Com* ast_atrib(char* var,Expr* e);
Com* ast_if(Expr* expr, List* next);
Com* ast_ifs(Expr* expr, List* next, Ifs* nextcondition);
Ifs* ast_elseif(Expr* expr, List* next, Ifs* nextcondition;);
Ifs* ast_else(List* next);
Ifs* ast_elif(Expr* ifexpr, List* ifnext);
List* ast_list(Com* cmds,List* next);

#endif