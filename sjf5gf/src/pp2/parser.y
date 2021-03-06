/* File: parser.y
 * --------------
 * Yacc input file to generate the parser for the compiler.
 *
 * pp2: your job is to write a parser that will construct the parse tree
 *      and if no parse errors were found, print it.  The parser should 
 *      accept the language as described in specification, and as augmented 
 *      in the pp2 handout.
 */

%{

/* Just like lex, the text within this first region delimited by %{ and %}
 * is assumed to be C/C++ code and will be copied verbatim to the y.tab.c
 * file ahead of the definitions of the yyparse() function. Add other header
 * file inclusions or C++ variable declarations/prototypes that are needed
 * by your code here.
 */
#include "scanner.h" // for yylex
#include "parser.h"
#include "errors.h"

void yyerror(const char *msg); // standard error-handling routine

%}

/* The section before the first %% is the Definitions section of the yacc
 * input file. Here is where you declare tokens and types, add precedence
 * and associativity options, and so on.
 */
 
/* yylval 
 * ------
 * Here we define the type of the yylval global variable that is used by
 * the scanner to store attibute information about the token just scanned
 * and thus communicate that information to the parser. 
 *
 * pp2: You will need to add new fields to this union as you add different 
 *      attributes to your non-terminal symbols.
 */
%union {
    int integerConstant;
    bool boolConstant;
    char *stringConstant;
    double doubleConstant;
    char identifier[MaxIdentLen+1]; // +1 for terminating null
    Decl *decl;
    VarDecl *var;
    FnDecl *fDecl;
    ClassDecl *classDecl;
    InterfaceDecl *interfaceDecl;
    Type *type;
    Stmt *stmt;
    List<Stmt*> *stmtList;
    List<VarDecl*> *varList;
    List<Decl*> *declList;
    List<Expr*> *exprList;
    NamedType *namedType;
    List<NamedType*> *namedTypeList;
    Expr *expr;
    Operator *oper;
    Call *call;
}


/* Tokens
 * ------
 * Here we tell yacc about all the token types that we are using.
 * Yacc will assign unique numbers to these and export the #define
 * in the generated y.tab.h header file.
 */
%token   T_Void T_Bool T_Int T_Double T_String T_Class 
%token   T_LessEqual T_GreaterEqual T_Equal T_NotEqual T_Dims T_Increment T_Decrement
%token   T_And T_Or T_Null T_Extends T_This T_Interface T_Implements
%token   T_While T_For T_If T_Else T_Return T_Break T_Switch
%token   T_New T_NewArray T_Print T_ReadInteger T_ReadLine

%token   <identifier> T_Identifier
%token   <stringConstant> T_StringConstant 
%token   <integerConstant> T_IntConstant
%token   <doubleConstant> T_DoubleConstant
%token   <boolConstant> T_BoolConstant


/* Non-terminal types
 * ------------------
 * In order for yacc to assign/access the correct field of $$, $1, we
 * must to declare which field is appropriate for the non-terminal.
 * As an example, this first type declaration establishes that the DeclList
 * non-terminal uses the field named "declList" in the yylval union. This
 * means that when we are setting $$ for a reduction for DeclList ore reading
 * $n which corresponds to a DeclList nonterminal we are accessing the field
 * of the union named "declList" which is of type List<Decl*>.
 * pp2: You'll need to add many of these of your own.
 */
%type <declList>  DeclList Field FieldList FieldItems
%type <exprList>  Actuals ActualList
%type <decl>      Decl
%type <type>      Type
%type <namedType> Extends NamedType
%type <namedTypeList> Implements NamedTypeList ImplementsList
%type <var>       Variable VarDecl
%type <varList>   Formals FormalList VarDecls
%type <fDecl>     FnDecl FnHeader
%type <stmtList>  StmtList
%type <stmt>      StmtBlock Stmt IfStmt WhileStmt ForStmt BreakStmt ReturnStmt PrintStmt LValue CaseStmt Constant PostfixStmt SwitchStmt
%type <classDecl> ClassDecl
%type <interfaceDecl> InterfaceDecl
%type <expr> Expr NewForStmt NewPrintStmt
%type <oper> operator Post
%type <call> Call
%nonassoc '='
%left '.'
%left '['
%%
/* Rules
 * -----
 * All productions and actions should be placed between the start and stop
 * %% markers which delimit the Rules section.
	 
 */
Program   :    DeclList            { 
                                      @1; 
                                      /* pp2: The @1 is needed to convince 
                                       * yacc to set up yylloc. You can remove 
                                       * it once you have other uses of @n*/
                                      Program *program = new Program($1);
                                      // if no errors, advance to next phase
                                      if (ReportError::NumErrors() == 0) 
                                          program->Print(0);
                                    }
          ;

DeclList  :    DeclList Decl        { ($$=$1)->Append($2); }
          |    Decl                 { ($$ = new List<Decl*>)->Append($1); };

Decl      :    VarDecl              { $$=$1; }
          |    FnDecl               { $$=$1; }
	  |    ClassDecl	    { $$=$1; }
	  |    InterfaceDecl	    { $$=$1; }
;

VarDecl   :    Variable ';'         { $$=$1; }
; 

Variable   :   Type T_Identifier    { $$ = new VarDecl(new Identifier(@2, $2), $1); }
;

Type      :    T_Int                { $$ = Type::intType; }
          |    T_Bool               { $$ = Type::boolType; }
          |    T_String             { $$ = Type::stringType; }
          |    T_Double             { $$ = Type::doubleType; }
          |    T_Identifier         { $$ = new NamedType(new Identifier(@1,$1)); }
          |    Type T_Dims          { $$ = new ArrayType(Join(@1, @2), $1); }
;

NamedType : T_Identifier	    {$$ = new NamedType(new Identifer(@1, $1));}
;

NamedTypeList: NamedTypeList NamedType {($$=$1)->append($2);}
	     | NamedType	       {($$ = new List<NamedType*>)->append($1);}
;
operator: T_LessEqual		{$$ = new Operator(@1,"<=");}
	| T_GreaterEqual	{$$ = new Operator(@1,">=");}
	| T_Equal		{$$ = new Operator(@1,"==");}
	| T_NotEqual		{$$ = new Operator(@1,"!=");}
	| T_And			{$$ = new Operator(@1,"&&");}
	| T_Or			{$$ = new Operator(@1,"||");}
	| T_Dims		{$$ = new Operator(@1,"[]");}
;

Post	: T_Increment		{$$ = new Operator(@1,"++");}
	| T_Decrement		{$$ = new Operator(@1,"--");}
;

FnDecl    :    FnHeader StmtBlock   { ($$=$1)->SetFunctionBody($2); }
;

FnHeader  :    Type T_Identifier '(' Formals ')'  
                                    { $$ = new FnDecl(new Identifier(@2, $2), $1, $4); }
          |    T_Void T_Identifier '(' Formals ')' 
                                    { $$ = new FnDecl(new Identifier(@2, $2), Type::voidType, $4); }
;

Formals   :    FormalList           { $$ = $1; }
          |    /* empty */          { $$ = new List<VarDecl*>; }
;
FormalList:    FormalList ',' Variable  
                                    { ($$=$1)->Append($3); }
          |    Variable             { ($$ = new List<VarDecl*>)->Append($1); }
;
ClassDecl: T_Class T_Identifier Extends Implements '{' Field '}' 
		{$$ = new ClassDecl(new Identifier(@2, $2), $3, $4, $6);}
;
Extends: T_Extends T_Identifier     {$$=new NamedType(new Identifier(@2, $2));}
       | {;}//empty		    
;
Implements: ImplementsList	{$$ = $1;}
	  | /* empty*/		{$$ = new List<NamedType*>;}
;
ImplementsList: ImplementsList ',' T_Identifier 
		{($$=$1)->Append(new NamedType(new Identifier(@3, $3) ) );}
	      | T_Implements T_Identifier 	
		{ ($$ = new List<NamedType*>)->Append(new NamedType(new Identifier(@2, $2)));}
;
Field: FieldList {$$=$1}
     |           {$$=new List<Decl*>;}
;
FieldList: FieldList FieldItems {($$=$1)->Append($2);};
FieldItems:	VarDecl {($$ = new List<Decl*>->Append($1);}
		| FnDecl {($$ = new List<Decl*>->Append($1);};
InterfaceDecl: T_Interface T_Identifier '{' Prototype '}' {;};
Prototype: Type T_Identifier '(' Formals ')' ';' | T_Void T_Identifier '(' Formals ')' ';'
		{;}
;

StmtBlock :    '{' VarDecls StmtList '}' 
                                    { $$ = new StmtBlock($2, $3); }
;

VarDecls  : VarDecls VarDecl     { ($$=$1)->Append($2); }
          | /* empty*/           { $$ = new List<VarDecl*>; }
;

StmtList  : StmtList Stmt { ($$=$1)->Append($2); } | Stmt {$$ = new List<Stmt*>;};
Stmt: NewForStmt ';' {$$=new Stmt();}| IfStmt {;}| WhileStmt {;}| ForStmt {;}| BreakStmt {;}| ReturnStmt {;}| PrintStmt {;}| StmtBlock {;} | PostfixStmt {;} | SwitchStmt {;};
IfStmt: T_If '(' Expr ')' Stmt ElseStmt{;};
ElseStmt: T_Else NewForStmt;
WhileStmt: T_While '(' Expr ')' Stmt
	{$$ = new WhileStmt(new Expr(@3), $5);}; 
ForStmt: T_For '(' NewForStmt ';' Expr ';' NewForStmt ')' Stmt
	{$$ = new ForStmt(new Expr(@3), new Expr(@5), new Expr(@7), new Stmt(@9));};
NewForStmt: Expr 
	{$$ = new Expr(@1);}| {;};
BreakStmt: T_Break ';' 
	{$$ = new BreakStmt(@1);};
ReturnStmt: T_Return NewForStmt ';' {;};
PrintStmt: T_Print '(' Expr NewPrintStmt ')' ';'
	{$$ = new Expr(@3);};
NewPrintStmt: ',' Expr 
	{$$ = new Expr(@2);} | {;} 
;
PostfixStmt: T_Identifier Post Stmt 
	{$$ = new PostfixStmt(new Expr(new Identifier(@1, $1)), $2);}
;
SwitchStmt: T_Switch '(' T_Identifier ')' CaseStmt
		{$$ = new SwitchStmt(new Expr(new Identifier(@3, $3)),$5);}
;
CaseStmt: CaseStmt ':' StmtList {$$ = new Stmt(@3);}
;
Expr:		LValue '=' Expr 
			{$$= new AssignExpr(new LValue(@1), new Operator(@2, '='), new Expr(@3));}
	|	Constant {$$=new Expr(@1);}
	|	LValue {$$ = new LValue(@1);}
	|	T_This {$$ = new This(@1);}
	|	Call{$$ = $1;}
	|	'(' Expr ')'{$$=$2;}
	|	T_Identifier '+' T_Identifier
			{$$ = new ArithmeticExpr(new Expr(@1), new Operator(@2, '+'), new Expr(@3));}
	|	T_Identifier '-' T_Identifier
			{$$ = new ArithmeticExpr(new Expr(@1), new Operator(@2, '-'), new Expr(@3));}
	|	T_Identifier '*' T_Identifier
			{$$ = new ArithmeticExpr(new Expr(@1), new Operator(@2, '*'), new Expr(@3));}
	|	T_Identifier '/' T_Identifier
			{$$ = new ArithmeticExpr(new Expr(@1), new Operator(@2, '/'), new Expr(@3));}
	|	T_Identifier '%' T_Identifier
			{$$ = new ArithmeticExpr(new Expr(@1), new Operator(@2, '%'), new Expr(@3));}
	|	'-' T_Identifier
			{$$ = new ArithmeticExpr(new Operator(@1, '-'), new Expr(@2));}
	|	T_Identifier '<' T_Identifier
			{$$ = new RelationalExpr(new Expr(@1), @2, new Expr(@3));}
	|	T_Identifier T_LessEqual T_Identifier
			{$$ = new RelationalExpr(new Expr(@1), @2, new Expr(@3));}
	|	T_Identifier '>' T_Identifier
			{$$ = new RelationalExpr(new Expr(@1), new Operator(@2, '>'), new Expr(@3));}
	|	T_Identifier T_GreaterEqual T_Identifier
			{$$ = new RelationalExpr(new Expr(@1), @2, new Expr(@3));}
	|	T_Identifier T_Equal T_Identifier
			{$$ = new EqualityExpr(new Expr(@1), @2, new Expr(@3));}
	|	T_Identifier T_NotEqual T_Identifier
			{$$ = new EqualityExpr(new Expr(@1), @2, new Expr(@3));}
	|	T_Identifier T_And T_Identifier
			{$$ = new LogicalExpr(new Expr(@1), @2, new Expr(@3));}
	|	T_Identifier T_Or T_Identifier
			{$$ = new LogicalExpr(new Expr(@1), @2, new Expr(@3));}
	|	'!' T_Identifier
			{$$ = new LogicalExpr(@1, new Expr(@2));}
	|	T_ReadInteger '(' ')' 
			{$$ = new ReadIntegerExpr(@1);}
	|	T_ReadLine '(' ')'
			{$$ = new ReadLineExpr(@1);}
	|	T_New '(' T_Identifier ')' 
			{$$ = new NewExpr(@2, new NamedType(new Identifier(@3,$3) ) );}
	|	T_NewArray '(' Expr ',' Type ')'
			{$$ = new NewArrayExpr(@1, $3, $5);}
;

LValue: 	T_Identifier {$$ = new LValue(new Identifier(@1, $1));}
	|	Expr '.' T_Identifier {$$ = new FieldAccess($1, new Identifier(@3, $3));}
	|	Expr '[' Expr ']' {$$ = new ArrayAccess(@1, $1, $3);}
;

Call:		T_Identifier '(' Actuals ')' {$$ = new Call(@1, new EmptyExpr(), new Identifier(@1, $1), $3);} 
	| 	Expr '.' T_Identifier '(' Actuals ')' 
		{$$ = new Call(@1, $1, new Identifier(@3, $3), $5);}
;

Actuals   :    ActualList           { $$ = $1; }
          |    /* empty */          { $$ = new List<Expr*>; }
;

ActualList:    ActualList ',' Expr  
                                    { ($$=$1)->Append($3); }
          |    Expr                 { ($$ = new List<Expr*>)->Append($1); }
;

Constant:	T_IntConstant {$$ = new IntConstant(@1, $1);}
	|	T_DoubleConstant {$$ = new DoubleConstant(@1, $1);}
	|	T_BoolConstant {$$ = new BoolConstant(@1, $1);}
	|	T_StringConstant {$$ = new StringConstant(@1, $1);}
	|	T_Null {$$ = new NullConstant(@1);}
;

%%

/* The closing %% above marks the end of the Rules section and the beginning
 * of the User Subroutines section. All text from here to the end of the
 * file is copied verbatim to the end of the generated y.tab.c file.
 * This section is where you put definitions of helper functions.
 */

/* Function: InitParser
 * --------------------
 * This function will be called before any calls to yyparse().  It is designed
 * to give you an opportunity to do anything that must be done to initialize
 * the parser (set global variables, configure starting state, etc.). One
 * thing it already does for you is assign the value of the global variable
 * yydebug that controls whether yacc prints debugging information about
 * parser actions (shift/reduce) and contents of state stack during parser.
 * If set to false, no information is printed. Setting it to true will give
 * you a running trail that might be helpful when debugging your parser.
 * Please be sure the variable is set to false when submitting your final
 * version.
 */
void InitParser()
{
   PrintDebug("parser", "Initializing parser");
   yydebug = false;
}
