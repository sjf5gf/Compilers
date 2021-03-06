/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "codegen.h"
#include <typeinfo>
#include <string>
using namespace std;

Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

void Program::Check() {
    /* You can use your pp3 semantic analysis or leave it out if
     * you want to avoid the clutter.  We won't test pp5 against 
     * semantically-invalid programs.
     */
}
void Program::Emit() {
    /* pp5: here is where the code generation is kicked off.
     *      The general idea is perform a tree traversal of the
     *      entire program, generating instructions as you go.
     *      Each node can have its own way of translating itself,
     *      which makes for a great use of inheritance and
     *      polymorphism in the node classes.
     */
    bool hasMain = false;
    for (int i = 0; i < decls->NumElements(); i++) {
        string temp ("main");
	string temp2 (decls->Nth(i)->id->name);
	if(temp.compare(temp2)==0) hasMain = true;
    }
    if(hasMain == false) {
	cout<<"***Linker: function 'main' not defined"<<endl;
	me->GenBuiltInCall(Halt);
    }
    else{
    decls->Emit();
    me->DoFinalCodeGen();
    }
}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
}

void StmtBlock::Emit() {
    decls->Emit();
    stmts->Emit();
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this); 
    (body=b)->SetParent(this);
}

void ConditionalStmt::Emit() {
	char * temp = me->NewLabel();
	me->GenIfZ(this->test->param, temp);
	me->GenGoto(temp);
	this->body->Emit();
	me->GenLabel(temp);
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) { 
    Assert(i != NULL && t != NULL && s != NULL && b != NULL);
    (init=i)->SetParent(this);
    (step=s)->SetParent(this);
}

void ForStmt::Emit() {
    init->Emit();
    char * temp = me->NewLabel();
    char * temp2 = me->NewLabel();
    me->GenLabel(temp);
    test->Emit();
    me->GenIfZ(this->test->param, temp);
    me->GenGoto(temp2);
    body->Emit();
    step->Emit();
    me->GenGoto(temp);
}

void WhileStmt::Emit() {
    char * temp = me->NewLabel();
    char * temp2 = me->NewLabel();
    me->GenLabel(temp);
    test->Emit();
    me->GenIfZ(this->test->param, temp);
    me->GenGoto(temp2);
    body->Emit();
    me->GenGoto(temp);
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}

void IfStmt::Emit() {
    this->test->Emit();
    char * temp = me->NewLabel();
    me->GenIfZ(this->test->param, temp);
    if (elseBody) {
	char *temp2 = me->NewLabel();
        me->GenGoto(temp2);
	this->body->Emit();
	me->GenGoto(temp);
	me->GenLabel(temp2);
	elseBody->Emit();
	me->GenLabel(temp);
    }
    else{
    me->GenGoto(temp);
    this->body->Emit();
    me->GenLabel(temp);
    }
}



ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    Assert(e != NULL);
    (expr=e)->SetParent(this);
}

void ReturnStmt::Emit() {
    expr->Emit();
    me->GenReturn(this->expr->param);
}
  
PrintStmt::PrintStmt(List<Expr*> *a) {    
    Assert(a != NULL);
    (args=a)->SetParentAll(this);
}

void PrintStmt::Emit() {
    args->Emit();
    me->GenPushParam(this->args->Nth(this->args->NumElements() -1)->param);
    string s = typeid(me->code.back()).name();
    string test ("int");
    string test2 ("bool");
    if(s.compare(test) == 0) me->GenLCall("_PrintInt", false);
    if(s.compare(test2) == 0) me->GenLCall("_PrintBool", false);
    else me->GenLCall("_PrintString", false);
}

Case::Case(IntConstant *v, List<Stmt*> *s) {
    Assert(s != NULL);
    value = v;
    if (value) value->SetParent(this);
    (stmts=s)->SetParentAll(this);
}

void Case::Emit() {
    if (value) value->Emit();
    stmts->Emit();
}
/*
void Case::PrintChildren(int indentLevel) {
    if (value) value->Print(indentLevel+1);
    stmts->PrintAll(indentLevel+1);
}*/

SwitchStmt::SwitchStmt(Expr *e, List<Case*> *c) {
    Assert(e != NULL && c != NULL);
    (expr=e)->SetParent(this);
    (cases=c)->SetParentAll(this);
}

void SwitchStmt::Emit() {
    expr->Emit();
    cases->Emit();
}
/*      
void SwitchStmt::PrintChildren(int indentLevel) {
    expr->Print(indentLevel+1);
    cases->PrintAll(indentLevel+1);
}*/


