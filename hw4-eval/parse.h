/*
 * parse.h
 */

/* GRAMMAR
Prog := Slist
Slist := Stmt SC { Slist }
Stmt := IfStmt | PrintStmt | Expr
 -+IfStmt := IF Expr THEN Stmt
 -+PrintStmt := PRINT Expr
Expr := LogicExpr { ASSIGN LogicExpr }
 -+LogicExpr := CompareExpr { (LOGICAND | LOGICOR) CompareExpr }
 -+CompareExpr := AddExpr { (EQ | NEQ | GT | GEQ | LT | LEQ) AddExpr }
AddExpr := MulExpr { (PLUS | MINUS) MulExpr }
 -+MulExpr := Factor { (STAR | SLASH) Factor }
Factor := MINUS Primary | Primary
 -+Primary := IDENT | ICONST | SCONST | TRUE | FALSE | LPAREN Expr
RPAREN
*/

#ifndef PARSE_H_
#define PARSE_H_

#include <iostream>
using namespace std;

#include "tokens.h"
#include "parsetree.h"

extern ParseTree *Prog(istream *in, int *line);
extern ParseTree *Slist(istream *in, int *line);
extern ParseTree *Stmt(istream *in, int *line);
extern ParseTree *IfStmt(istream *in, int *line);
extern ParseTree *PrintStmt(istream *in, int *line);
extern ParseTree *Expr(istream *in, int *line);
extern ParseTree *LogicExpr(istream *in, int *line);
extern ParseTree *CompareExpr(istream *in, int *line);
extern ParseTree *AddExpr(istream *in, int *line);
extern ParseTree *MulExpr(istream *in, int *line);
extern ParseTree *Factor(istream *in, int *line);
extern ParseTree *Primary(istream *in, int *line);

#endif /* PARSE_H_ */
