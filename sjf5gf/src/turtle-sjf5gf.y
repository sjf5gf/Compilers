
%{
#include <stdio.h>
#include "symtab.h"
%}

%union { int i; node *n; double d;}

%token GO TURN VAR JUMP
%token FOR STEP TO DO
%token COPEN CCLOSE
%token SIN COS SQRT
%token <d> FLOAT
%token <n> ID               
%token <i> NUMBER       
%token SEMICOLON PLUS MINUS TIMES DIV OPEN CLOSE ASSIGN LESSTHAN GREATERTHAN EQUAL IF ELSE OPENBRACKET CLOSEBRACKET WHILE LEQ GEQ THEN

%type <n> decl
%type <n> decllist

%%
program: head decllist stmtlist tail;

head: { printf("%%!PS Adobe\n"
               "\n"
	       "newpath 0 0 moveto\n"
	       );
      };

tail: { printf("closepath\nstroke\n"); };

decllist: ;
decllist: decllist decl;

decl: VAR ID SEMICOLON { printf("/tlt%s 0 def\n",$2->symbol);} ;

stmtlist: ;
stmtlist: stmtlist stmt ;

stmt: ID ASSIGN expr SEMICOLON {printf("/tlt%s exch store\n",$1->symbol);};
stmt: GO expr SEMICOLON {printf("0 rlineto\n");};
stmt: JUMP expr SEMICOLON {printf("0 rmoveto\n");};
stmt: TURN expr SEMICOLON {printf("rotate\n");};
stmt: ID {printf("tlt%s ",$1->symbol);} LESSTHAN expr {printf("lt\n",$1->symbol);};
stmt: VAR LESSTHAN expr {printf("lt\n");};
stmt: ID {printf("tlt%s ",$1->symbol);} GREATERTHAN expr {printf("gt\n",$1->symbol);};
stmt: VAR GREATERTHAN expr {printf("gt\n");};
stmt: ID {printf("tlt%s ",$1->symbol);} EQUAL expr {printf("eq\n",$1->symbol);};
stmt: VAR EQUAL expr {printf("eq\n");};
stmt: ID ASSIGN expr SEMICOLON {printf("/tlt%s exch store\n",$1->symbol);};
stmt: VAR ASSIGN expr;
stmt: ID {printf("tlt%s ",$1->symbol);} LEQ expr {printf("le\n",$1->symbol);};
stmt: VAR LEQ expr {printf("le\n");};
stmt: ID {printf("tlt%s ",$1->symbol);} GEQ expr {printf("ge\n",$1->symbol);};
stmt: VAR GEQ expr{printf("ge\n");};

stmt: FOR ID ASSIGN expr 
          STEP expr
	  TO expr
	  DO {printf("{ /tlt%s exch store\n",$2->symbol);} 
	     stmt {printf("} for\n");};

stmt: COPEN stmtlist CCLOSE;	 

expr: expr PLUS term { printf("add ");};
expr: expr MINUS term { printf("sub ");};
expr: term;

term: term TIMES factor { printf("mul ");};
term: term DIV factor { printf("div ");};
term: factor;

factor: MINUS atomic { printf("neg ");};
factor: PLUS atomic;
factor: SIN factor { printf("sin ");};
factor: COS factor { printf("cos ");};
factor: SQRT factor { printf("sqrt ");};
factor: atomic;



atomic: OPEN expr CLOSE;
atomic: NUMBER {printf("%d ",$1);};
atomic: FLOAT {printf("%f ",$1);};
atomic: ID {printf("tlt%s ", $1->symbol);};

stmt: WHILE OPEN {printf("{ ");} stmt {printf("{} {exit} ifelse\n");} CLOSE OPENBRACKET stmtlist CLOSEBRACKET {printf("} loop\n");};

stmt: IF OPEN stmt {printf("{ ");} CLOSE THEN OPENBRACKET stmtlist CLOSEBRACKET {printf("} ");} newstmt;
newstmt: ELSE OPENBRACKET {printf("{ ");} stmtlist CLOSEBRACKET {printf("} ifelse\n");};
newstmt: {printf("if\n");};

%%
int yyerror(char *msg)
{  fprintf(stderr,"Error: %s\n",msg);
   return 0;
}

int main(void)
{   yyparse();
    return 0;
}

