/* File: ast.cc
 * ------------
 */

#include "ast.h"
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h> // strdup
#include <stdio.h>  // printf
#include "hashtable.h"
#include "errors.h"
#include <iostream>
using namespace std;
extern Hashtable<Decl*> *table;
extern Hashtable<Identifier*> *iTable;
Node::Node(yyltype loc) {
    location = new yyltype(loc);
    parent = NULL;
}

Node::Node() {
    location = NULL;
    parent = NULL;
}

/* The Print method is used to print the parse tree nodes.
 * If this node has a location (most nodes do, but some do not), it
 * will first print the line number to help you match the parse tree 
 * back to the source text. It then indents the proper number of levels 
 * and prints the "print name" of the node. It then will invoke the
 * virtual function PrintChildren which is expected to print the
 * internals of the node (itself & children) as appropriate.
 */
void Node::Print(int indentLevel, const char *label) { 
    const int numSpaces = 3;
    printf("\n");
    if (GetLocation()) 
        printf("%*d", numSpaces, GetLocation()->first_line);
    else 
        printf("%*s", numSpaces, "");
    printf("%*s%s%s: ", indentLevel*numSpaces, "", 
           label? label : "", GetPrintNameForNode());
   PrintChildren(indentLevel);
}

void Node::Build(int indentLevel, const char *label){
    const int numSpaces = 3;
    BuildChildren(indentLevel);
    Check();
}

void Node::Check(){
	Iterator<Decl*> iter = table->GetIterator();
	Decl *decl;
	Iterator<Decl*> iter2 = table->GetIterator();
	while ((decl = iter.GetNextValue()) != NULL) {
		Decl *dummy;
		dummy = iter2.GetNextValue();
		Decl *decl2;
		while((decl2 = iter2.GetNextValue()) != NULL){
		if(decl == decl2 && table->Lookup(decl->id->name) == table->Lookup(decl2->id->name)) ReportError::DeclConflict(decl, decl2);}
	}
}
	 
Identifier::Identifier(yyltype loc, const char *n) : Node(loc) {
    name = strdup(n);
} 

void Identifier::PrintChildren(int indentLevel) {
    printf("%s", name);
}

void Identifier::BuildChildren(int indentLevel) {
	iTable->Enter(name,this,false);    
	//printf("%s", name);
}
