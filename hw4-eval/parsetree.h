/*
 * parsetree.h
 */

#ifndef PARSETREE_H_
#define PARSETREE_H_

#include <vector>
#include <map>
#include "value.h"
using std::vector;
using std::map;

extern std::map<std::string, Value> evars;
extern void RunTimeError (string);

// NodeType represents all possible types
enum NodeType { ERRTYPE, INTTYPE, STRTYPE, BOOLTYPE, IDENTTYPE };

// a "forward declaration" for a class to hold values
class Value;

class ParseTree {
    int         linenum;
    
protected:
    ParseTree   *left;
    ParseTree   *right;

public:
    ParseTree(int linenum, ParseTree *l = 0, ParseTree *r = 0)
        : linenum(linenum), left(l), right(r) {}

    virtual ~ParseTree() {
        delete left;
        delete right;
    }

    virtual Value Eval() { return Value(); }

    int GetLinenum() const { return linenum; }

    virtual NodeType GetType() const { return ERRTYPE; }
    virtual string GetId() const { return ""; }
    virtual bool IsIdent() const { return false; }

    int LeafCount() const {
        int lc = 0;
        if( left ) lc += left->LeafCount();
        if( right ) lc += right->LeafCount();
        if( left == 0 && right == 0 )
            lc++;
        return lc;
    }
    
    // other methods
    int StringCount() const {
    int sc = 0;
    if( left ) sc += left->StringCount();
    if( right ) sc += right->StringCount();
    if((*this).GetType() == STRTYPE)
        sc++;
    return sc;
}
    int IdentCount() const {
    int ic = 0;
    if( left ) ic += left->IdentCount();
    if( right ) ic += right->IdentCount();
    if((*this).GetType() == IDENTTYPE)
        ic++;
    return ic;
}
};

class StmtList : public ParseTree {

public:
    StmtList(ParseTree *l, ParseTree *r) : ParseTree(0, l, r) {}

    Value Eval() {
        if(left) {
            left->Eval();
        }
        if(right) {
            right->Eval();
        }
        return Value();
    }

};

class IfStatement : public ParseTree {
public:
    IfStatement(int line, ParseTree *ex, ParseTree *stmt) : ParseTree(line, ex, stmt) {}

    Value Eval() {
        Value results = left->Eval();
        if(results.isBoolType() == true) {
            if(results.getBoolean()) {
                return right->Eval();  
            }
        } else {
            RunTimeError("if expression is not boolean typed");
        }
            return Value();
            //return 0;
    }
};

class Assignment : public ParseTree {
    string id;
    Value val;

public:
    Assignment(int line, ParseTree *lhs, ParseTree *rhs) : ParseTree(line, lhs, rhs) {}

    Value Eval() {
        if(left->IsIdent()) {
            Value r1 = right->Eval();
            string l1 = left->GetId();
            evars[l1] = r1;
            return r1;
        } else {
            RunTimeError("Left hand side must be an identifier");
        }
        return Value();
    }
};

class PrintStatement : public ParseTree {
public:
    PrintStatement(int line, ParseTree *e) : ParseTree(line, e) {}
    
    Value Eval() override {
            Value v = left->Eval();
            if (v.isIntType()) {
                cout << v.getInteger() << endl;
            }
            else if (v.isStringType()) {
                cout << v.getString() << endl;
            }
            else if (v.isBoolType()) {
                cout << v << endl;
            }
            return Value();
    }
};

class PlusExpr : public ParseTree {
public:
    PlusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}

    Value Eval() {
        auto a1 = left->Eval();
        auto a2 = right->Eval();
        return a1 + a2; // uses overloaded operator+
        }
};

class MinusExpr : public ParseTree {
public:
    MinusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}

    Value Eval() {
        auto a1 = left->Eval();
        auto a2 = right->Eval();
        return a1 - a2; // uses overloaded operator-
        }

};

class TimesExpr : public ParseTree {
public:
    TimesExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}

    Value Eval() {
        auto t1 = left->Eval();
        auto t2 = right->Eval();
        return t1 * t2; //uses overloaded operator*
    }
};

class DivideExpr : public ParseTree {
public:
    DivideExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}

    Value Eval() {
        auto d1 = left->Eval();
        auto d2 = right->Eval();
        return d1 / d2; //user overloaded operator/
    }
};

class LogicAndExpr : public ParseTree {
public:
    LogicAndExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    
    Value Eval() {
        Value left1 = left->Eval();
        Value right1 = right->Eval();
        if (left1.isBoolType() && right1.isBoolType()) {
            return left1.isTrue() && right1.isTrue();
        } else {
            RunTimeError("first operand of && is not boolean typed");
        }
        return Value();
    }
};

class LogicOrExpr : public ParseTree {
public:
    LogicOrExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    
    Value Eval() {
        Value or1 = left->Eval();
        Value or2 = right->Eval();
        if (or1.isBoolType() || or2.isBoolType()) {
            return or1.isTrue() || or2.isTrue();
        } else {
            RunTimeError("first operand of || is not boolean typed");
        }
        return Value();
    }
};

class EqExpr : public ParseTree {
public:
    EqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}

    NodeType GetType() const {return BOOLTYPE;}
    Value Eval() {
        return left->Eval() == right->Eval();
    }
};

class NEqExpr : public ParseTree {
public:
    NEqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    
    Value Eval() {
        return left->Eval() != right->Eval();
    }
};

class LtExpr : public ParseTree {
public:
    LtExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}

    Value Eval() {
        return left->Eval() < right->Eval();
    }
};

class LEqExpr : public ParseTree {
public:
    LEqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}

    Value Eval() {
        return left->Eval() <= right->Eval();
    }
};

class GtExpr : public ParseTree {
public:
    GtExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}

    Value Eval() {
        return left->Eval() > right->Eval();
    }
};

class GEqExpr : public ParseTree {
public:
    GEqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}

    Value Eval() {
        return left->Eval() >= right->Eval();
    }
};

class IConst : public ParseTree {
    int val;

public:
    IConst(int l, int i) : ParseTree(l), val(i) {}
    IConst(Token& t) : ParseTree(t.GetLinenum()) {
        val = stoi(t.GetLexeme());
    }

    NodeType GetType() const { return INTTYPE; }
    
    Value Eval() {
        return Value(val);
    }
};

class BoolConst : public ParseTree {
    bool val;

public:
    BoolConst(Token& t, bool val) : ParseTree(t.GetLinenum()), val(val) {}

    NodeType GetType() const { return BOOLTYPE; }

    Value Eval() {
        Value v = Value(val);
        //std::cout << v.isBoolType() << std::endl;
        return Value(v);
    }
};

class SConst : public ParseTree {
    string val;

public:
    SConst(Token& t) : ParseTree(t.GetLinenum()) {
        val = t.GetLexeme();
    }

    NodeType GetType() const { return STRTYPE; }

    Value Eval() {
        return Value(val);
    }
};

class Ident : public ParseTree {
    string id;

public:
    Ident(Token& t) : ParseTree(t.GetLinenum()), id(t.GetLexeme()) {}
    //add ident return type
    bool IsIdent() const override {return true;}
    string GetId() const override {return id;}
    
    Value Eval() {
        if(!evars.empty() && evars.count(id)) {
            return evars[id];
        } else {
            RunTimeError("Variable is used before being set");
            return Value();
        } 
        return Value();
    }
};

#endif /* PARSETREE_H_ */
