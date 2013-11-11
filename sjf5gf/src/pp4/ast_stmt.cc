/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "hashtable.h"
#include "ast.h"
#include <iostream>
#include <string>
using namespace std;
Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
    dino = new Scope(this);
    
}

void Program::PrintChildren(int indentLevel) {
    decls->PrintAll(indentLevel+1);
    printf("\n");
}

void Program::BuildChildren(){
    decls->BuildScope(this->dino);
    this->Check1();
    decls->CheckScope(this->dino); 
}

void Program::Check1(){
	Iterator<Node*> iter = this->dino->table->GetIterator();
	Decl* decl;
	int count = 1;
	while((decl = dynamic_cast<Decl *>(iter.GetNextValue())) != NULL){
		Decl* decl2;
		Iterator<Node*> iter2 = this->dino->table->GetIterator();
		for(int a = 0; a < count; a++){decl2 = dynamic_cast<Decl *>(iter2.GetNextValue());}
		while((decl2 = dynamic_cast<Decl *>(iter2.GetNextValue())) != NULL){
			string s1(decl->id->name);  string s2 (decl2->id->name);			
			if((s1).compare(s2) == 0) ReportError::DeclConflict(decl, decl2);
		}
	count++;	
	}
}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
    dino = new Scope(this);
}

void StmtBlock::PrintChildren(int indentLevel) {
    decls->PrintAll(indentLevel+1);
    stmts->PrintAll(indentLevel+1);
}

void StmtBlock::BuildChildren() {
    decls->BuildScope(this->dino);
    //stmts->BuildScope(this->dino);
}

void StmtBlock::Check1() {
	Iterator<Node*> iter = this->dino->table->GetIterator();
	Decl* decl;
	int count = 1;
	while((decl = dynamic_cast<Decl *>(iter.GetNextValue())) != NULL){
		Decl* decl2;
		Iterator<Node*> iter2 = this->dino->table->GetIterator();
		for(int a = 0; a < count; a++){decl2 = dynamic_cast<Decl *>(iter2.GetNextValue());}
		while((decl2 = dynamic_cast<Decl *>(iter2.GetNextValue())) != NULL){
			string s1(decl->id->name);  string s2 (decl2->id->name);			
			if((s1).compare(s2) == 0) ReportError::DeclConflict(decl, decl2);
		}
	count++;	
	}
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
    init->Build();
    test->Build();
    step->Build();
    body->Build();
}

void WhileStmt::PrintChildren(int indentLevel) {
    test->Print(indentLevel+1, "(test) ");
    body->Print(indentLevel+1, "(body) ");
}

void WhileStmt::BuildChildren(int indentLevel) {
    test->Build();
    body->Build();
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
    test->Build();
    body->Build();
    if (elseBody) elseBody->Build();
}

ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    Assert(e != NULL);
    (expr=e)->SetParent(this);
}

void ReturnStmt::PrintChildren(int indentLevel) {
    expr->Print(indentLevel+1);
}

void ReturnStmt::BuildChildren(int indentLevel) {
    expr->Build();
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
}

void Case::PrintChildren(int indentLevel) {
    if (value) value->Print(indentLevel+1);
    stmts->PrintAll(indentLevel+1);
}

void Case::BuildChildren(int indentLevel) {
    if (value) value->Build();
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
    expr->Build();
    cases->CaseBuildAll(indentLevel+1);
}


