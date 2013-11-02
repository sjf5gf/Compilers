/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "hashtable.h"

Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
    //Hashtable<Decl*> *table = new Hashtable<Decl*>;
}

void Program::PrintChildren(int indentLevel) {
    decls->PrintAll(indentLevel+1);
    printf("\n");
}

void Program::BuildChildren(int indentLevel) {
    decls->BuildAll(indentLevel+1);
    //printf("\n");
}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
    //Hashtable<Stmt*> *stmtTable = new Hashtable<Stmt*>;
}

void StmtBlock::PrintChildren(int indentLevel) {
    decls->PrintAll(indentLevel+1);
    stmts->PrintAll(indentLevel+1);
}

void StmtBlock::BuildChildren(int indentLevel) {
    decls->BuildAll(indentLevel+1);
    stmts->StmtBuildAll(indentLevel+1);
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this); 
    (body=b)->SetParent(this);
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) { 
    Assert(i != NULL && t != NULL && s != NULL && b != NULL);
    (init=i)->SetParent(this);
    (step=s)->SetParent(this);
}

void ForStmt::PrintChildren(int indentLevel) {
    init->Print(indentLevel+1, "(init) ");
    test->Print(indentLevel+1, "(test) ");
    step->Print(indentLevel+1, "(step) ");
    body->Print(indentLevel+1, "(body) ");
}

void ForStmt::BuildChildren(int indentLevel) {
    init->Build(indentLevel+1, "(init) ");
    test->Build(indentLevel+1, "(test) ");
    step->Build(indentLevel+1, "(step) ");
    body->Build(indentLevel+1, "(body) ");
}

void WhileStmt::PrintChildren(int indentLevel) {
    test->Print(indentLevel+1, "(test) ");
    body->Print(indentLevel+1, "(body) ");
}

void WhileStmt::BuildChildren(int indentLevel) {
    test->Build(indentLevel+1, "(test) ");
    body->Build(indentLevel+1, "(body) ");
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}

void IfStmt::PrintChildren(int indentLevel) {
    test->Print(indentLevel+1, "(test) ");
    body->Print(indentLevel+1, "(then) ");
    if (elseBody) elseBody->Print(indentLevel+1, "(else) ");
}

void IfStmt::BuildChildren(int indentLevel) {
    test->Build(indentLevel+1, "(test) ");
    body->Build(indentLevel+1, "(then) ");
    if (elseBody) elseBody->Build(indentLevel+1, "(else) ");
}

ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    Assert(e != NULL);
    (expr=e)->SetParent(this);
}

void ReturnStmt::PrintChildren(int indentLevel) {
    expr->Print(indentLevel+1);
}

void ReturnStmt::BuildChildren(int indentLevel) {
    expr->Build(indentLevel+1);
}
  
PrintStmt::PrintStmt(List<Expr*> *a) {    
    Assert(a != NULL);
    (args=a)->SetParentAll(this);
}

void PrintStmt::PrintChildren(int indentLevel) {
    args->PrintAll(indentLevel+1, "(args) ");
}

void PrintStmt::BuildChildren(int indentLevel) {
    args->ExprBuildAll(indentLevel+1, "(args) ");
}

Case::Case(IntConstant *v, List<Stmt*> *s) {
    Assert(s != NULL);
    value = v;
    if (value) value->SetParent(this);
    (stmts=s)->SetParentAll(this);
    //Hashtable<Case*> *caseTable = new Hashtable<Case*>;
}

void Case::PrintChildren(int indentLevel) {
    if (value) value->Print(indentLevel+1);
    stmts->PrintAll(indentLevel+1);
}

void Case::BuildChildren(int indentLevel) {
    if (value) value->Build(indentLevel+1);
    stmts->StmtBuildAll(indentLevel+1);
}

SwitchStmt::SwitchStmt(Expr *e, List<Case*> *c) {
    Assert(e != NULL && c != NULL);
    (expr=e)->SetParent(this);
    (cases=c)->SetParentAll(this);
}
      
void SwitchStmt::PrintChildren(int indentLevel) {
    expr->Print(indentLevel+1);
    cases->PrintAll(indentLevel+1);
}

void SwitchStmt::BuildChildren(int indentLevel) {
    expr->Build(indentLevel+1);
    cases->CaseBuildAll(indentLevel+1);
}


