/*
 * parse.cpp
 */

#include "parse.h"
#include "parsetree.h"
#include "tokens.h"
#include <string.h>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

/* GRAMMAR
Prog := Slist
Slist := Stmt SC { Slist }
Stmt := IfStmt | PrintStmt | Expr
IfStmt := IF Expr THEN Stmt
PrintStmt := PRINT Expr
Expr := LogicExpr { ASSIGN LogicExpr }
LogicExpr := CompareExpr { (LOGICAND | LOGICOR) CompareExpr }
CompareExpr := AddExpr { (EQ | NEQ | GT | GEQ | LT | LEQ) AddExpr }
AddExpr := MulExpr { (PLUS | MINUS) MulExpr }
MulExpr := Factor { (STAR | SLASH) Factor }
Factor := MINUS Primary | Primary
Primary := IDENT | ICONST | SCONST | TRUE | FALSE | LPAREN Expr
RPAREN
*/

//int stringct = 0;

std::string tempWord = "";
std::map<std::string, int> idents;
int identct = 0;

// WRAPPER FOR PUSHBACK
namespace Parser {
	bool pushed_back = false;
	Token	pushed_token;

	static Token GetNextToken(istream *in, int *line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(Token& t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;
	}

}

static int error_count = 0;

void ParseError(int line, string msg) {
	++error_count;
	cout << line << ": " << msg << endl;
}
// Prog := Slist
ParseTree *Prog(istream *in, int *line) {
	//:= Slist
	ParseTree *sl = Slist(in, line);

	if( sl == 0 ) //empty branch
		ParseError(*line, "No statements in program");

	if( error_count )
		return 0;

	return sl;
}
//Slist := Stmt SC { Slist }
// Slist is a Statement followed by a Statement List
ParseTree *Slist(istream *in, int *line) {
	// Stmt SC { Slist }
	ParseTree *s = Stmt(in, line);
	if( s == 0 )
		return 0;
	//SC { Slist }
	if( Parser::GetNextToken(in, line) != SC ) {
		ParseError(*line, "Missing semicolon");
		return 0;
	}
	//{ Slist }
    return new StmtList(s, Slist(in,line));
}
// Stmt := IfStmt | PrintStmt | Expr
ParseTree *Stmt(istream *in, int *line) {
	ParseTree *s;
	//:= IfStmt | PrintStmt | Expr
	Token t = Parser::GetNextToken(in, line);
	switch( t.GetTokenType() ) {
	case IF:
		s = IfStmt(in, line);
		break;

	case PRINT:
		s = PrintStmt(in, line);
		break;

	case DONE:
		return 0;

	case ERR:
		ParseError(*line, "Invalid token");
		return 0;

	default:
		// put back the token and then assume it's an Expr
		Parser::PushBackToken(t);
		s = Expr(in, line);
		if( s == 0 ) { //no right leaf, Expr
			ParseError(*line, "Invalid statement");
			return 0;
		}
		break;
	}


	return s;
}

ParseTree *IfStmt(istream *in, int *line) {
	//IfStmt := IF Expr THEN Stmt
    //assume next node is Expr
    ParseTree *ex = Expr(in, line);

    //Expr THEN Stmt
    if (!ex) {
		ParseError(*line, "Expected IF condition (boolean expression)");
		return 0;
	}
	//THEN Stmt
	Token t = Parser::GetNextToken(in, line);
	if(t != THEN ) {
		ParseError(*line, "Expected THEN after Expr");
		return 0;
	}
	//Stmt
	ParseTree *stmt = Stmt(in, line);

    return new IfStatement(*line, ex, stmt);
}
//PrintStmt := PRINT Expr
ParseTree *PrintStmt(istream *in, int *line) {
    // ADD HANDLER
    //:= PRINT Expr
    ParseTree *ex = Expr(in, line);
    if(!ex) { //no right brach, this isn't optional
    	ParseError(*line, "Bad prt stmt");
    	return 0;
    }
	
	return new PrintStatement(ex->GetLinenum(), ex);
}
//Expr := LogicExpr { ASSIGN LogicExpr }
ParseTree *Expr(istream *in, int *line) {
	//Expr := LogicExpr
	ParseTree *t1 = LogicExpr(in, line);
	if( t1 == 0 ) {
		return 0;
	}
	// { ASSIGN 
	Token t = Parser::GetNextToken(in, line);

	if( t != ASSIGN ) {
		Parser::PushBackToken(t);
		return t1;
	}
	// LogicExpr }
	ParseTree *t2 = Expr(in, line); // right assoc
	if( t2 == 0 ) {
		ParseError(*line, "Missing expression after operator");
		return 0;
	}

	return new Assignment(t.GetLinenum(), t1, t2);
}
//LogicExpr := CompareExpr { (LOGICAND | LOGICOR) CompareExpr }
ParseTree *LogicExpr(istream *in, int *line) {
	ParseTree *t1 = CompareExpr(in, line);
	if( t1 == 0 ) {
		return 0;
	}
	//loop for recursion
    while (true) {
        //{ (LOGICAND | LOGICOR) CompareExpr }
        Token t = Parser::GetNextToken(in, line);
        if( (t != LOGICAND) && (t != LOGICOR)) {
            Parser::PushBackToken(t);
            return t1;
        }

        ParseTree *t2 = LogicExpr(in, line); // right assoc
        
        if( t2 == 0 ) { //CompareExpr is req after LOGIC[A-Z]*
            ParseError(*line, "Missing Compare");
            return 0;
        }
        //LOGICAND | LOGICOR
        if( t == LOGICAND ) {
                t1 = new LogicAndExpr(t.GetLinenum(), t1, t2);
        } else {
                t1 = new LogicOrExpr(t.GetLinenum(), t1, t2);
        }
    }
}
//CompareExpr := AddExpr { (EQ | NEQ | GT | GEQ | LT | LEQ) AddExpr }
ParseTree *CompareExpr(istream *in, int *line) {
	//:= AddExpr
	ParseTree *t1 = AddExpr(in, line);
	if( t1 == 0 ) {
		return 0;
	}

    // HANDLE OP
    while (true) {
    	Token t = Parser::GetNextToken(in, line);
		//{ (EQ | NEQ | GT | GEQ | LT | LEQ) 
    	if(t != EQ && t != NEQ && t != LT && t != LEQ && t != GT && t != GEQ) {
    		Parser::PushBackToken(t);
    		return t1;
    	}

    	//right node AddExpr }
    	ParseTree *t2 = CompareExpr(in, line);
    	//its not a valid expr because there is no right leaf
    	if(t2 == 0) {
    		ParseError(*line, "Missing expression after operator");
    		return 0;
    	}
    	//(EQ | NEQ | GT | GEQ | LT | LEQ)
    	//return op: Token(tt, lexeme, int)
    	switch(t.GetTokenType()) {
    		case EQ:
    			return new EqExpr(t.GetLinenum(), t1, t2);
    		case NEQ:
    			return new NEqExpr(t.GetLinenum(), t1, t2); 
    		case LT:
    			return new LtExpr(t.GetLinenum(), t1, t2);
    		case LEQ:
    			return new LEqExpr(t.GetLinenum(), t1, t2);
    		case GT:
    			return new GtExpr(t.GetLinenum(), t1, t2);
    		case GEQ:
    			return new GEqExpr(t.GetLinenum(), t1, t2);
    		default:
    			return 0;

    	}
    }
    //return 0;
}
//AddExpr := MulExpr { (PLUS | MINUS) MulExpr }
ParseTree *AddExpr(istream *in, int *line) {
	// := MulExpr
	ParseTree *t1 = MulExpr(in, line);
	if( t1 == 0 ) {
		return 0;
	}
	//loop for recursion
	while ( true ) {
		
		Token t = Parser::GetNextToken(in, line);
		//(PLUS | MINUS)
		if( t != PLUS && t != MINUS ) {
			Parser::PushBackToken(t);
			return t1;
		}
		// MulExpr } recursion
		ParseTree *t2 = MulExpr(in, line);
		// MulExpr { (PLUS | MINUS) } MulExpr
		if( t2 == 0 ) { //no right node, also true
			ParseError(*line, "Missing expression after operator");
			return 0; //true
		}
		//{ (PLUS | MINUS) }
		if( t == PLUS )
			t1 = new PlusExpr(t.GetLinenum(), t1, t2);
		else
			t1 = new MinusExpr(t.GetLinenum(), t1, t2);
	}
}

ParseTree *MulExpr(istream *in, int *line) {
	ParseTree *t1 = Factor(in, line);
	if( t1 == 0 ) {
		return 0;
	}

    // HANDLE OP
    //loop for recursion
    while (true) {
	    Token t = Parser::GetNextToken(in, line);
	    
	    if(t != STAR && t != SLASH) {
			Parser::PushBackToken(t);
			return t1;
	    }
	    //2nd factor
	    ParseTree *t2 = Factor(in, line);
			if( t2 == 0 ) {
				ParseError(*line, "Missing expression after operator");
				return 0;
			}
			//return the expressions
			if( t == STAR )
				t1 = new TimesExpr(t.GetLinenum(), t1, t2);
			else
				t1 = new DivideExpr(t.GetLinenum(), t1, t2);

	}
}
//Factor := MINUS Primary | Primary
ParseTree *Factor(istream *in, int *line) {
	bool neg = false;
	Token t = Parser::GetNextToken(in, line);
	//lexer literally just does negatives by interpreting subtraction
	if( t == MINUS ) {
		neg = true;
	}
	else {
		Parser::PushBackToken(t);
	}
	//| Primary
	ParseTree *p1 = Primary(in, line);
	if( p1 == 0 ) { //no node MINUS needs a Primary right leaf
		ParseError(*line, "Missing primary");
		return 0;
	}

	if( neg ) {
        // handle as -1 * Primary
		return new TimesExpr(t.GetLinenum(), new IConst(t.GetLinenum(), -1), p1);
	}
	else
		return p1;
}
//Primary := 
ParseTree *Primary(istream *in, int *line) {
	Token t = Parser::GetNextToken(in, line);
    // PROCESS TOKEN, IDENTIFY PRIMARY, RETURN SOMETHING
    // IDENT | ICONST | SCONST | TRUE | FALSE | LPAREN Expr
    switch(t.GetTokenType()) {
    	case IDENT:
    	{
	    	identct++;
	    	tempWord = t.GetLexeme();
	    	idents[tempWord]++;
	        return new Ident(t);
    	}
    	case ICONST:
    		return new IConst(t);
    	case SCONST:
    		return new SConst(t);
    	case TRUE:
    		return new BoolConst(t, TRUE);
    	case FALSE:
    		return new BoolConst(t, FALSE);
    	case LPAREN:
    	{
    		Token t = Parser::GetNextToken(in, line);
			
			if( t != RPAREN ) {
				ParseError(t.GetLinenum(), "Syntax error right paren expected");
				Parser::PushBackToken(t);
				return 0;
			} else {
				return ex;
			}
    	}
    	default:
    		return 0; //empty branch
    }
    /*if(t == IDENT){
    	identct++;
    	tempWord = t.GetLexeme();
    	idents[tempWord]++;
        return new Ident(t);
    } else if(t == ICONST) {
        return new IConst(t);
    } else if(t == SCONST) {
        return new SConst(t);
    } else if(t == TRUE) {
        return new BoolConst(t, TRUE);
    } else if(t == FALSE) {
        return new BoolConst(t, FALSE);
    } else if(t == LPAREN) {
        ParseTree *ex = Expr(in, line); //only accepts Expr
        // Expr { RPAREN }
        Token t = Parser::GetNextToken(in, line);
		if( t != RPAREN ) {
			ParseError(t.GetLinenum(), "Syntax error right paren expected");
			Parser::PushBackToken(t);
			return 0;
		} else {
			return ex;
		}
	} else {
		ParseError(t.GetLinenum(), "Syntax error primary expected");
	}   
    return 0; //empty branch*/

}//Parsetree

void ParseOut(ParseTree *prog) {
	//
	//cout << identct << endl;
	cout << "LEAF COUNT: " << prog->LeafCount() << endl;
	cout << "STRING COUNT: " << prog->StringCount() << endl;
	
	if(identct != 0) {
		cout << "IDENT COUNT: " << identct << endl;
		int maxcount = 0;
		std::string currid = "";
		//std::vector<std::string> top_idents;
		//std::map<std::string, int>
		for(auto it = idents.begin(); it != idents.end();it++) {
	        if(it->second > maxcount) {
	             maxcount = it->second;
	             currid = it->first;
	             //top_idents.push_back(currid);
	        } else if(it->second == maxcount) {
	             currid += ", " + it->first;   
	        }
    	}
    	cout << currid << endl;
    }
}