#include <stdio.h>
#include "parser.h"
#include "code.h"
#include <stdlib.h>
#include <string.h>

char ** vars;

int nv,lab,elses; // registos, labels

void truble(char* name){
  int i=0;
  while(vars[i]!=NULL){
    if(!strcmp(vars[i],name)) return;
    i++;
  }
  vars[i]=(char*) malloc(32*sizeof(char));
  strcpy(vars[i],strdup(name));
  return;
} // check new vars for .data

Addr* mkVaddr(char* name) {
  Addr* ad =(Addr*) malloc(sizeof(Addr));
  ad->op = Var;
  ad->content.name = malloc(20*sizeof(char));
  strcpy(ad->content.name,name);

  if(name[0]!='$' && name[0]!='L') truble(name); // catch new var to add on data

  return ad;
} // create VAR adress
  
Addr* mkIaddr(int val){
  Addr* ad = malloc(sizeof(Addr));
  ad->op = Int;
  ad->content.val = val;
  return ad;
} // create INT adress

char* newchar(int i){
  char str1[20];
  char str2[20];
  strcpy(str1,"$t");
  sprintf(str2, "%d", i); 
  char * str3 = (char *) malloc(1 + strlen(str1)+ strlen(str2) );
  strcpy(str3, str1);
  strcat(str3, str2);

  return str3;
} // create a new reg

char* newLabel(int i){

  char str1[20];
  char str2[20];
  strcpy(str1,"L");
  sprintf(str2, "%d", i); 
  char * str3 = (char *) malloc(1 + strlen(str1)+ strlen(str2) );
  strcpy(str3, str1);
  strcat(str3, str2);

  return str3;
} //create Labels

char* newElse(int i){ 

  char str1[20];
  char str2[20];
  strcpy(str1,"E");
  sprintf(str2, "%d", i); 
  char * str3 = (char *) malloc(1 + strlen(str1)+ strlen(str2) );
  strcpy(str3, str1);
  strcat(str3, str2);

  return str3;
} //create label for elses

IList* joinLists(IList* l1,IList* l2){
  IList* p;
  if(l1==NULL) return l2;
  for(p = l1; p->next!=NULL; p=p->next);
  p->next = l2;
  return l1;
} //conc 2 list

Istr* mkIstr(Opkind op, Addr* addr1, Addr* addr2, Addr* addr3){

  Istr* node = (Istr*) malloc(sizeof(Istr));
  node->addr1 = addr1;
  node->addr2 = addr2;
  node->addr3 = addr3;
  node->op=op;
  return node;
} //create an Intruction

IList* mkList(Istr* head,IList* tail){

  IList* newp = (IList*) malloc(sizeof(IList));
  newp->inst=head;
  newp->next=tail;
  return newp;
} //create a List
 
Pair* CE(Expr* expr){ 
  
  Pair* p = (Pair*) malloc(sizeof(Pair));

  if (expr == 0) {yyerror("Null expression!!");}

  else if (expr->kind == E_INTEGER) {
    p->name=mkIaddr(expr->attr.value);
    p->il=NULL;
    return p;
  }

  else if (expr->kind == E_VAR) {
    p->name = mkVaddr(expr->attr.name);
    p->il=NULL;
    return p;
  }

  else if (expr->kind == E_OPERATION) { // BOOLEANS AND ARITHMETIC
    Istr* t;
    IList *t1,*t2;
    Addr *a1,*a2;

    Pair* p1 =CE(expr->attr.op.left);
    if(p1->name->op==Int)    {a1=mkVaddr(newchar(nv));t1=mkList(mkIstr(I_LI,a1,p1->name,NULL),p1->il);nv++;} // if is INT li
    else if(p1->name->content.name[0]!='$'){a1=mkVaddr(newchar(nv));t1=mkList(mkIstr(I_LW,a1,p1->name,NULL),p1->il);nv++;} // if is a VAR and not a REG
    else{t1=p1->il; a1 = p1->name;}// if reg nothing is need

    Pair* p2 =CE(expr->attr.op.right);
    if(p2->name->op==Int)    {a2=mkVaddr(newchar(nv));t2=mkList(mkIstr(I_LI,a2,p2->name,NULL),p2->il);nv++;}// if is INT li
    else if(p2->name->content.name[0]!='$'){a2=mkVaddr(newchar(nv));t2=mkList(mkIstr(I_LW,a2,p2->name,NULL),p2->il);nv++;} // if is a VAR and not a REG   
    else{t2=p2->il;a2 = p2->name;}// if reg nothing is need

    switch(expr->attr.op.operator){
      case PLUS:  t = mkIstr(I_PLUS,  mkVaddr(newchar(nv)),a1,a2); break;
      case MINUS: t = mkIstr(I_MINUS, mkVaddr(newchar(nv)),a1,a2); break;
      case MUL:   t = mkIstr(I_MUL,   mkVaddr(newchar(nv)),a1,a2); break;
      case DIV:   t = mkIstr(I_DIV,   mkVaddr(newchar(nv)),a1,a2); break;
      case EQ:    t = mkIstr(I_EQ,    mkVaddr(newchar(nv)),a1,a2); break;
      case DIF:   t = mkIstr(I_DIF,   mkVaddr(newchar(nv)),a1,a2); break;
      case MORE:  t = mkIstr(I_MORE,  mkVaddr(newchar(nv)),a1,a2); break;
      case LESS:  t = mkIstr(I_LESS,  mkVaddr(newchar(nv)),a1,a2); break;
      case MOEQ:  t = mkIstr(I_MOEQ,  mkVaddr(newchar(nv)),a1,a2); break;
      case LEEQ:  t = mkIstr(I_LEEQ,  mkVaddr(newchar(nv)),a1,a2); break;
      default: printf("ERROR of PARSER\n");
    }

    p->name=mkVaddr(newchar(nv)); nv++;
    p->il=joinLists(joinLists(t1,t2),mkList(t,NULL)); //append the lists

    return p;
  }
} // Compile Expressions

Pair* CompileExpression(Expr* expr){
  Pair* p = CE(expr); //Expression to atribute
  Pair* t = (Pair*) malloc(sizeof(Pair));

  if(p->name->op==Int) {t->name=mkVaddr(newchar(nv));t->il=mkList(mkIstr(I_LI,t->name,p->name,NULL),p->il);nv++;} // if is INT li
  else if(p->name->content.name[0]!='$') {t->name=mkVaddr(newchar(nv));t->il=mkList(mkIstr(I_LW,t->name,p->name,NULL),p->il);nv++;} // if is a VAR and not a REG
  else{t->il=p->il; t->name=p->name;}// if reg nothing is need

  return t;
} //CE helper

IList* CompileCom(Com* command){
  //ALL atribuitions outside switch!
  IList* l=NULL;
  IList* t=NULL;
  Pair* p =NULL;
  char* L0=NULL;
  char* L1=NULL;
  char* L2=NULL;
  Istr* i =NULL;
  List* help = NULL;
  Addr* reg=NULL;
  Addr* reg2=NULL;
  Addr* v=NULL;

  switch(command->kind){

    case C_ATR: //DONE
      // struct{char* var; struct Expression* expr;} atrib;

      p = CompileExpression(command->attr.atrib.expr); //Expression to atribute

      reg = mkVaddr(newchar(nv));nv++; //reg where var will be saved
      v =  mkVaddr(command->attr.atrib.var);
      i = mkIstr(I_LW,reg,v,NULL); // reg = var

      t = mkList(i,p->il);
      l = joinLists(t,mkList(mkIstr(I_ATRIB,reg,p->name,NULL),NULL));
      l = joinLists(l,mkList(mkIstr(I_SW,reg,v,NULL),NULL)); //save VAR

      return l;


    case C_IF: // DONE
      //struct Expression* expr; struct CommandList* next;} iff;

      lab++; L0 = newLabel(lab); //Label for begin of if
      lab++; L1 = newLabel(lab); //Label for end of if

      p = CompileExpression(command->attr.iff.expr); 

      l = joinLists(p->il,mkList(mkIstr(I_IF,p->name,mkVaddr(L0),NULL),NULL)); //evaluate the expression
      l = joinLists(l,mkList(mkIstr(I_GOTO,mkVaddr(L1),NULL,NULL),NULL));
      l = joinLists(l,mkList(mkIstr(I_LABEL,mkVaddr(L0),NULL,NULL),NULL)); // if expression true come here

      help=command->attr.iff.next; //COMMANDLIST to iterate the commands inside the if
      while(help!=NULL){ t=CompileCom(help->command); help=help->next; l = joinLists(l,t);} //join all comands INSIDE the if to Ilist

      l = joinLists(l,mkList(mkIstr(I_LABEL,mkVaddr(L1),NULL,NULL),NULL)); //OUT OF IF

      return l;


    case C_IFS: // DONE
      //struct Expression* expr; struct CommandList* next; struct IfList* nextcondition;} ifs;

      lab++; L0 = newLabel(lab); //Label for Begin of If
      lab++; L1 = newLabel(lab); //Label for End of If
      char * E1 = newElse(elses); elses++; // Label for end of all ifs -> can ONLY be define here!!!

      p = CompileExpression(command->attr.ifs.expr); 

      l = joinLists(p->il,mkList(mkIstr(I_IF,p->name,mkVaddr(L0),NULL),NULL)); //evaluate the expression 
      l = joinLists(l,mkList(mkIstr(I_GOTO,mkVaddr(L1),NULL,NULL),NULL)); // if expression fail go to the end

      //if condition TRUE down

      l = joinLists(l,mkList(mkIstr(I_LABEL,mkVaddr(L0),NULL,NULL),NULL)); // if expression true come here
      help=command->attr.ifs.next; //COMMANDLIST to iterate the commands inside the if
      while(help!=NULL){ t=CompileCom(help->command); help=help->next; l = joinLists(l,t);} //join all comands INSIDE the if to Ilist
      l = joinLists(l,mkList(mkIstr(I_GOTO,mkVaddr(E1),NULL,NULL),NULL)); //end of if go after all ifs
     
      //if condirion true up

      l = joinLists(l,mkList(mkIstr(I_LABEL,mkVaddr(L1),NULL,NULL),NULL)); //OUT OF IF
      t = CompileIfs(command->attr.ifs.nextcondition,E1); //Ilist of next elseif condition
      l = joinLists(l,t); //join the Lists
    
      return l;


    case C_WHILE: // DONE
      //struct{struct Expression* expr; struct CommandList* next;} whiles;

      p = CompileExpression(command->attr.whiles.expr); //expression to evaluate

      lab++; L0 = newLabel(lab);  //Label for begin of the loop
      lab++; L1 = newLabel(lab); //label for end of cycle
      lab++; L2 = newLabel(lab); //label for loop without condition

      l = joinLists(mkList(mkIstr(I_LABEL,mkVaddr(L0),NULL,NULL),NULL),p->il); //CREATE LABEL TO BEGIN OF LOOP
      l = joinLists(l,mkList(mkIstr(I_IF,p->name,mkVaddr(L2),NULL),NULL)); //evaluate the expression 
      l = joinLists(l,mkList(mkIstr(I_GOTO,mkVaddr(L1),NULL,NULL),NULL));  //if condition fail go to end of loop

      // WHILE TRUE DOWN

      l = joinLists(l,mkList(mkIstr(I_LABEL,mkVaddr(L2),NULL,NULL),NULL)); //CREATE LABEL TO BEGIN OF LOOP without condition
      help=command->attr.whiles.next; //COMMANDLIST to iterate the commands inside the loop
      while(help!=NULL){ t=CompileCom(help->command); help=help->next; l = joinLists(l,t);} //join all comands INSIDE the loop to Ilist    
      l = joinLists(l,mkList(mkIstr(I_GOTO,mkVaddr(L0),NULL,NULL),NULL)); // SEND LOOP TO BEGIN

      // WHILE true above

      l = joinLists(l,mkList(mkIstr(I_LABEL,mkVaddr(L1),NULL,NULL),NULL)); //OUT OF LOOP

      return l;


    case C_FOR: // DONE
      // struct{char* var; int i; struct Expression* expr; struct CommandList* next;} fors; 

      p = CompileExpression(command->attr.fors.expr);

      reg = mkVaddr(newchar(nv)); nv++;//reg where var will be saved
      v =  mkVaddr(command->attr.fors.var);
      reg2 = mkVaddr(newchar(nv)); nv++;

      l= mkList(mkIstr(I_LW,reg,v,NULL),mkList(mkIstr(I_LI,reg2,mkIaddr(command->attr.fors.i),NULL),mkList(mkIstr(I_ATRIB,reg,reg2,NULL),mkList(mkIstr(I_SW,reg,v,NULL),NULL))));
      // last line was just i=0...
      
      lab++; L0 = newLabel(lab);  //label for begin of loop
      lab++; L1 = newLabel(lab); //label for END of FOR
      lab++; L2 = newLabel(lab);  //label for begin of loop after condition

      l = joinLists(l,mkList(mkIstr(I_LABEL,mkVaddr(L0),NULL,NULL),NULL)); //Create Label for begin of loop
      l = joinLists(l,p->il);
      l = joinLists(l,mkList(mkIstr(I_LW,reg,v,NULL),NULL));
      l = joinLists(l,mkList(mkIstr(I_LEEQ,reg2,reg,p->name),NULL));
      l = joinLists(l,mkList(mkIstr(I_IF,reg2,mkVaddr(L2),NULL),NULL)); //evaluate the expression 
      l = joinLists(l,mkList(mkIstr(I_GOTO,mkVaddr(L1),NULL,NULL),NULL)); //if condition fail go to end of loop
      l = joinLists(l,mkList(mkIstr(I_LABEL,mkVaddr(L2),NULL,NULL),NULL)); //CREATE LABEL TO BEGIN OF LOOP without condition

      help=command->attr.fors.next; //COMMANDLIST to iterate the commands inside the loop
      while(help!=NULL){ t=CompileCom(help->command); help=help->next; l = joinLists(l,t);} //join all comands INSIDE the loop to Ilist
      
      l = joinLists(l,mkList(mkIstr(I_LW,reg,v,NULL),NULL));
      l = joinLists(l,mkList(mkIstr(I_LI,reg2,mkIaddr(1),NULL),NULL));
      l = joinLists(l,mkList(mkIstr(I_PLUS,reg,reg,reg2),NULL));
      l = joinLists(l,mkList(mkIstr(I_SW,reg,v,NULL),NULL));
      //last 4 i++ so fun...

      l = joinLists(l,mkList(mkIstr(I_GOTO,mkVaddr(L0),NULL,NULL),NULL)); // SEND LOOP TO BEGIN

      // INSIDE FOR LOOP ABOVE

      l = joinLists(l,mkList(mkIstr(I_LABEL,mkVaddr(L1),NULL,NULL),NULL)); //OUT OF LOOP

      return l;


    case C_OUT: //DONE
      //struct{struct Expression* expr;} out;
      
      p = CompileExpression(command->attr.out.expr);  //EXPRESSION
      l = joinLists(p->il,mkList(mkIstr(I_OUTPUT,p->name,NULL,NULL),NULL));

      return l;


    case C_IN: //DONE
      // struct{char* name;} in; 

      l = mkList(mkIstr(I_INPUT,mkVaddr(command->attr.in.name),mkVaddr(newchar(nv)),NULL),NULL);
nv++;
      return l;

    }
}

IList* CompileIfs(Ifs* command,char * E){
  IList* l=NULL;
  IList* t=NULL;
  Pair* p =NULL;
  char* L0=NULL;
  char* L1=NULL;
  char* L2=NULL;
  Istr* i = NULL;
  List* help = NULL;

  switch(command->kind){

    case IF_ELSEIF:// DONE
      //struct {struct Expression* expr; struct CommandList* next; struct IfList* nextcondition;} elseifs;

      lab++; L0 = newLabel(lab); //Label for Begin of If
      lab++; L1 = newLabel(lab);  //Label for End of If

      p = CompileExpression(command->attr.elseifs.expr); 

      l = joinLists(p->il,mkList(mkIstr(I_IF,p->name,mkVaddr(L0),NULL),NULL)); //evaluate the expression -> mkIntr esta mal
      l = joinLists(l,mkList(mkIstr(I_GOTO,mkVaddr(L1),NULL,NULL),NULL));

      // IF expression true down

      l = joinLists(l,mkList(mkIstr(I_LABEL,mkVaddr(L0),NULL,NULL),NULL)); // if expression true come here
      help=command->attr.elseifs.next; //COMLIST to iterate the commands inside the if
      while(help!=NULL){ t=CompileCom(help->command); help=help->next; l = joinLists(l,t);} //join all comands INSIDE the if to Ilist
      l = joinLists(l,mkList(mkIstr(I_GOTO,mkVaddr(E),NULL,NULL),NULL)); //end of if go after all ifs

      // IF Expression TRUE UP

      l = joinLists(l,mkList(mkIstr(I_LABEL,mkVaddr(L1),NULL,NULL),NULL)); //OUT OF IF
      t = CompileIfs(command->attr.elseifs.nextcondition,E); //Print next condition 
      l = joinLists(l,t); //join the Lists

      return l;


    case IF_ELSE: //DONE
      //struct {struct CommandList* next;} elses;

      help=command->attr.elses.next; //COMLIST 
      while(help!=NULL){ t = CompileCom(help->command); help=help->next;l=joinLists(l,t);}

      l = joinLists(l,mkList(mkIstr(I_LABEL,mkVaddr(E),NULL,NULL),NULL)); //end of if go after all ifs

      return l;


    case IF_ELIF: // IF EVALUATE ERROR
      //struct {struct Expression* expr; struct CommandList* next;} elif;

      lab++; L0 = newLabel(lab); //Label for Begin of If

      p = CompileExpression(command->attr.elif.expr); 

      l = joinLists(p->il,mkList(mkIstr(I_IF,p->name,mkVaddr(L0),NULL),NULL)); //evaluate the expression 
      l = joinLists(l,mkList(mkIstr(I_GOTO,mkVaddr(E),NULL,NULL),NULL));
      l = joinLists(l,mkList(mkIstr(I_LABEL,mkVaddr(L0),NULL,NULL),NULL)); // if expression true come here

      help=command->attr.elif.next; //COMLIST for iterate ...
      while(help!=NULL){ t = CompileCom(help->command); help=help->next;joinLists(l,t);}

      l = joinLists(l,mkList(mkIstr(I_LABEL,mkVaddr(E),NULL,NULL),NULL)); //end of if go after all ifs

      return l;

  }
}

void writeToassembly(FILE* f,IList* i){
  //I_PLUS,I_MINUS,I_DIV,I_MUL,I_EQ,I_DIF,I_MORE,I_LESS,I_MOEQ,I_LEEQ,I_ATRIB,I_IF,I_GOTO,I_LABEL,I_INPUT,I_OUTPUT

  if(i==NULL) return;
  
  Istr* ii=i->inst;

  char *s1, *s2, *s3;
  s1=(char*) malloc(20*sizeof(char));
  s2=(char*) malloc(20*sizeof(char));
  s3=(char*) malloc(20*sizeof(char));

  if(ii->addr1!=NULL){ 
    if(ii->addr1->op==Int) sprintf(s1,"%d", ii->addr1->content.val); 
    else{
      strcat(s1,strdup(ii->addr1->content.name)); }  
  }

  if(ii->addr2!=NULL){ 
    if(ii->addr2->op==Int){
      sprintf(s2,"%d", ii->addr2->content.val);
    } 
    else{
      strcat(s2,strdup(ii->addr2->content.name));
    } 
  }

  if(ii->addr3!=NULL){
    if(ii->addr3->op==Int){ 
      sprintf(s3,"%d", ii->addr3->content.val);
    } 
    else{
      strcat(s3,strdup(ii->addr3->content.name));
    } 
  }


  switch(ii->op){

    case I_PLUS:
      fprintf(f,"add %s, %s, %s \n",ii->addr1->content.name,s2,s3);
      
      break;

    case I_MINUS:
      fprintf(f,"sub %s, %s, %s \n",s1,s2,s3);
      break;

    case I_DIV:
      fprintf(f,"div %s, %s, %s \n",s1,s2,s3);
      break;

    case I_MUL:
      fprintf(f,"mul %s, %s, %s\n",s1,s2,s3);
      break;

    case I_EQ:
      fprintf(f,"seq %s, %s, %s\n",s1,s2,s3); //set equal
      break;

    case I_DIF:
      fprintf(f,"sne %s, %s, %s\n",s1,s2,s3); // set not equal
      break;

    case I_MORE:
      fprintf(f,"sgt %s, %s, %s\n",s1,s2,s3); // set greater 
      break;

    case I_LESS:
      fprintf(f,"slt %s, %s, %s\n",s1,s2,s3); // set less 
      break;

    case I_MOEQ:
      fprintf(f,"sge %s, %s, %s\n",s1,s2,s3); //set greater or equal
      break;

    case I_LEEQ:
      fprintf(f,"sle %s, %s, %s\n",s1,s2,s3); // set ledd or equal 
      break;
      
    case I_ATRIB:
      fprintf(f,"add %s, %s, $0\n",s1,s2); 
      break;
      
    case I_IF:
      fprintf(f, "bne %s, $0, %s\n",s1,s2);  //branch not equal
      break;

    case I_LABEL:
      fprintf(f,"%s:\n",s1); 
      break;

    case I_GOTO:
      fprintf(f, "j %s\n",s1); //JUMP 
      break;

    case I_INPUT:
      fprintf(f,"addi $v0 , $0 , 5\n"); 
      fprintf(f,"syscall\n");   //get input INT
      fprintf(f,"move %s, $v0 \n",s2);
      fprintf(f,"sw %s, %s\n",s2,s1); 
      break;

    case I_OUTPUT:
      fprintf(f,"addi $v0 , $0 , 1\n");
      fprintf(f,"la $a0 , (%s)\n",s1);
      fprintf(f,"syscall\n");     //printing INT
      fprintf(f,"addi $a0, $0, 0xA\n");
      fprintf(f,"addi $v0, $0, 0xB\n");
      fprintf(f,"syscall\n");
      break;


    case I_SW:
      fprintf(f, "sw %s, %s\n",s1,s2);
      break;

    case I_LI:
     fprintf(f, "li %s, %s\n",s1,s2);
      break;

    case I_LW: 
      fprintf(f, "lw %s, %s\n",s1,s2);
      break;

    default: 
      fprintf(f, "SOME ERROR APPEND ON ASSEMBLE");
      break;
  }

  writeToassembly(f,i->next); // go to next Instruction
}

void startWrite(FILE* f){
  fprintf(f, ".data\n");
  int i=0;
  while(vars[i]!=NULL){
    fprintf(f,"  %s: .word 4\n",vars[i]);
    i++;
  }
  fprintf(f,".text\n");
}
void closeWrite(FILE* f){
  fprintf(f, "li $v0, 10\n");
  fprintf(f, "syscall\n");
}

int main(int argc, char** argv) {
  IList* i = NULL;
  vars = (char**)malloc( 512*sizeof(char*));
  --argc; ++argv;
  if (argc != 0) {
    yyin = fopen(*argv, "r");
    if (!yyin) { printf("'%s': could not open file\n", *argv); return 1; }
  } 

  FILE *f = fopen(argv[1], "w+");
  if (f == NULL)  { printf("Error opening OUTPUT file!\n"); exit(1);}

  if (yyparse() == 0) {
      while(root != NULL){
        nv=0;
        i = joinLists(i,CompileCom(root->command)); /*i vai ser a lista de instrucoes*/
        root = root->next;
    }
  }
  startWrite(f);
  writeToassembly(f,i);
  closeWrite(f);
  fclose(f);
  return 0;
}
