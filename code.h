#ifndef __code_h__
#define __code_h__

typedef enum{Int,Var} Addrkind;

struct Adress{
  Addrkind op;
  union{
    int val;
    char* name;
  }content;
};

typedef enum{I_PLUS,I_MINUS,I_DIV,I_MUL, //ARITMETIC OPERATION
            I_EQ,I_DIF,I_MORE,I_LESS,I_MOEQ,I_LEEQ, //BOOLEAN OPERATORS
            I_ATRIB,I_IF,I_GOTO,I_LABEL, //COMMAND INSTRUCTIONS 
            I_INPUT,I_OUTPUT, // I/O COMMANDS 
            I_SW, I_LI, I_LW} Opkind; // Acess To memory and LI

struct Instruction{
  Opkind op;
  struct Adress* addr1,* addr2,* addr3;
};

struct InstructionList{ 
  struct Instruction * inst;
  struct InstructionList* next;
};

struct ExpressionPair{
  struct Adress* name;
  struct InstructionList * il;
};

typedef struct InstructionList IList;
typedef struct Instruction Istr;
typedef struct Adress Addr;
typedef struct ExpressionPair Pair;

Istr* mkIstr(Opkind op, Addr* addr1, Addr* addr2, Addr* addr3);
IList* mkList(Istr* head,IList* tail);
IList* joinLists(IList* l1,IList* l2); //apend 2 ILists
Pair* CompileExpr(Expr* e);
IList* CompileCom(Com* c);
IList* CompileIfs(Ifs* c,char* E);
Addr* mkVaddr(char* name);
Addr* mkIaddr(int val);
#endif