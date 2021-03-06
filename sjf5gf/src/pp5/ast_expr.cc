/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "errors.h"
#include <string.h>
#include <iostream>
using namespace std;

IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
}

void IntConstant::Emit() {
	param = me->GenLoadConstant(this->value);
}

int IntConstant::getValue() {
	return this->value;
}

DoubleConstant::DoubleConstant(yyltype loc, double val) : Expr(loc) {
    value = val;
}

void DoubleConstant::Emit(){

}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
}

void BoolConstant::Emit() {
	if(this->value == true) param = me->GenLoadConstant(1);
	else param = me->GenLoadConstant(0);
}

int BoolConstant::getValue() {
	if(this->value == false) return 0;
	else return 1;
}

StringConstant::StringConstant(yyltype loc, const char *val) : Expr(loc) {
    Assert(val != NULL);
    value = strdup(val);
}

void StringConstant::Emit() {
	param = me->GenLoadConstant(this->value);
}

const char * StringConstant::getStringValue() {	
	string str = "";
	str.append(this->value);
	char * cstr = new char [str.length()+1];
	strcpy(cstr, str.c_str());	
	return cstr;
}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
    opName = tok;
}

void Operator::Emit() {

}
CompoundExpr::CompoundExpr(Expr *l, Operator *o, Expr *r) 
  : Expr(Join(l->GetLocation(), r->GetLocation())) {
    Assert(l != NULL && o != NULL && r != NULL);
    (op=o)->SetParent(this);
    (left=l)->SetParent(this); 
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Operator *o, Expr *r) 
  : Expr(Join(o->GetLocation(), r->GetLocation())) {
    Assert(o != NULL && r != NULL);
    left = NULL; 
    (op=o)->SetParent(this);
    (right=r)->SetParent(this);
}

void CompoundExpr::Emit() {

}

void AssignExpr::Emit(){
	me->GenAssign(me->GenLoadLabel(this->left->getStringValue()), me->GenLoadConstant(this->right->getValue()));
}

void ArithmeticExpr::Emit(){
	me->GenBinaryOp(this->op->opName, me->GenLoadConstant(this->left->getValue()), me->GenLoadConstant(this->right->getValue()));
}

void RelationalExpr::Emit(){
	me->GenBinaryOp(this->op->opName, me->GenLoadConstant(this->left->getValue()), me->GenLoadConstant(this->right->getValue()));
}

void EqualityExpr::Emit(){
	me->GenBinaryOp(this->op->opName, me->GenLoadConstant(this->left->getValue()), me->GenLoadConstant(this->right->getValue()));
}

void LogicalExpr::Emit(){
	me->GenBinaryOp(this->op->opName, me->GenLoadConstant(this->left->getValue()), me->GenLoadConstant(this->right->getValue()));
}
    
ArrayAccess::ArrayAccess(yyltype loc, Expr *b, Expr *s) : LValue(loc) {
    (base=b)->SetParent(this); 
    (subscript=s)->SetParent(this);
}

void ArrayAccess::Emit() {
	if(this->subscript->getValue() > this->base->getSize()) {
		cout<<"Decaf runtime error: Array subscript out of bounds"<<endl;
		me->GenBuiltInCall(Halt);
	}
	else me->GenLoad(this->base->param, this->subscript->getValue() * 4);
}
     
FieldAccess::FieldAccess(Expr *b, Identifier *f) 
  : LValue(b? Join(b->GetLocation(), f->GetLocation()) : *f->GetLocation()) {
    Assert(f != NULL); // b can be be NULL (just means no explicit base)
    base = b; 
    if (base) base->SetParent(this); 
    (field=f)->SetParent(this);
}

void FieldAccess::Emit() {
	me->GenLoad(this->base->param);
}

Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
    Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
    base = b;
    if (base) base->SetParent(this);
    (field=f)->SetParent(this);
    (actuals=a)->SetParentAll(this);
}

void Call::Emit() {
	for(int i = 0; i<this->actuals->NumElements(); i++){
	this->actuals->Nth(i)->Emit();
	Location *param = this->actuals->Nth(i)->param;
	me->GenPushParam(param);
	}
	me->GenLCall(this->field->name, true);
}
 
NewExpr::NewExpr(yyltype loc, NamedType *c) : Expr(loc) { 
  Assert(c != NULL);
  (cType=c)->SetParent(this);
}

void NewExpr::Emit() {

}


NewArrayExpr::NewArrayExpr(yyltype loc, Expr *sz, Type *et) : Expr(loc) {
    Assert(sz != NULL && et != NULL);
    (size=sz)->SetParent(this); 
    (elemType=et)->SetParent(this);
}

void NewArrayExpr::Emit() {
	if(this->size->getValue() <= 0) {
		cout<<"Decaf runtime error: Array size is <= 0"<<endl;
		me->GenBuiltInCall(Halt);
	}
}

int NewArrayExpr::getSize() {
	return this->size->getValue();
}

PostfixExpr::PostfixExpr(LValue *lv, Operator *o) : Expr(Join(lv->GetLocation(), o->GetLocation())) {
    Assert(lv != NULL && o != NULL);
    (lvalue=lv)->SetParent(this);
    (op=o)->SetParent(this);
}

void PostfixExpr::Emit() {

}
/*
void PostfixExpr::PrintChildren(int indentLevel) {
    lvalue->Print(indentLevel+1);
    op->Print(indentLevel+1);
}*/
       
