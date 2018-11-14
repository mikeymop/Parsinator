#include <map>
#include <string.h>
#include <cctype>
#include "tokens.h"

//track all tokentypes from tokens.h
static std::map<TokenType, std::string> TokenMap {
    {PRINT, "PRINT"},
    {IF, "IF"},
    {THEN, "THEN"},
    {TRUE, "TRUE"},
    {FALSE, "FALSE"},
    {IDENT, "IDENT"},
    {ICONST, "ICONST"},
    {SCONST, "SCONST"},
    {PLUS, "PLUS"},
    {MINUS, "MINUS"},
    {STAR, "STAR"},
    {SLASH, "SLASH"},
    {ASSIGN, "ASSIGN"},
    {EQ, "EQ"},
    {NEQ, "NEQ"},
    {LT, "LT"},
    {LEQ, "LEQ"},
    {GT, "GT"},
    {GEQ, "GEQ"},
    {LOGICAND, "LOGICAND"},
    {LOGICOR, "LOGICOR"},
    {LPAREN, "LPAREN"},
    {RPAREN, "RPAREN"},
    {SC, "SC"},
    {ERR, "ERR"},
    {DONE, "DONE"},
};

//I have to treat idents diff, since I cant read the ident 
//tokens in the TokenMap

Token identificator(const string& lexeme, int lineNum) {
    TokenType tt = IDENT;
    if(lexeme == "print") {
        tt = PRINT;
    }
    if(lexeme == "if") {
        tt = IF;
    }
    if(lexeme == "then") {
        tt = THEN;
    }
    if(lexeme == "true") {
        tt = TRUE;
    }
    if(lexeme == "false") {
        tt = FALSE;
    }
    return Token(tt, lexeme, lineNum);
}
//redir istream to cout
ostream& operator<<(ostream& out, const Token &tok) {
    TokenType tt = tok.GetTokenType();
    //output whatever token you match from the map
    out << TokenMap[tt];
    //check a few tokentypes including string and int
    if(tt == IDENT || tt == SCONST || tt == ICONST || tt == ERR) {
        out << "(" << tok.GetLexeme() << ")";
    }
    return out;
}

Token getNextToken(istream *in, int *lineNum) {
    //from slide
    enum LexState {BEGIN, INID, INSTRING, ININT, INCOMMENT, /* do more later */};
    string lexeme; //the lexeme we're building
    char ch; //the char we're working with
    LexState state = BEGIN; //sets the start node of FSM
    
    //deref instream
    while(in->get(ch)) {
        //grab chars / close lines
        if(ch == '\n') {
            (*lineNum)++;
        }
        //lexemes for strings and declarations
        switch(state) {
            //begin case starts the FSM
            case BEGIN:
                if(isspace(ch)) {
                    continue; //move on to next word if you see a space
                }
                
                lexeme = ch; //adds char to lexeme
                //Identify TokenType
                if(isalpha(ch)) {
                    state = INID;
                } else if(isdigit(ch)) {
                    state = ININT;
                } else if( ch == '"' ) {
                    state = INSTRING;
                } else if( ch == '#') {
                    state = INCOMMENT;
                } else {
                    // operators
                    TokenType tt = ERR;
                    switch(ch) {
                        case '+':
                            tt = PLUS;
                            break; 
                        case '*':
                            tt = STAR;
                            break;
                        case '/':
                            tt = SLASH;
                            break;
                        case '-':
                            tt = MINUS;
                            break;
                        case '=':
                            //two char ops are strange
                            lexeme.push_back(ch);
                            ch = in->peek();
                            if(ch == '=') {
                                lexeme.push_back(ch);
                                in->get(ch);
                                tt = EQ;
                            } else {
                                tt = ASSIGN;
                            }
                            break;
                        case '!':
                        //two char ops are strange
                            lexeme.push_back(ch);
                            ch = in->peek();
                            if(ch == '=') {
                                lexeme.push_back(ch);
                                in->get(ch);
                                tt = NEQ;
                            } else if(ch == '\n') {
                                (*lineNum)++;
                                lexeme.pop_back();
                                return Token(ERR, lexeme, *lineNum);
                            } else {
                                return Token(ERR, lexeme, *lineNum);
                            }
                            break;
                        case '<':
                            //two char ops are strange
                            lexeme.push_back(ch);
                            ch = in->peek();
                            if(ch == '=') {
                                lexeme.push_back(ch);
                                in->get(ch);
                                tt = LEQ;
                            }
                            tt = LT;
                            break;
                        case '>':
                            //two char ops are strange
                            lexeme.push_back(ch);
                            ch = in->peek();
                            if(ch == '=') {
                                lexeme.push_back(ch);
                                in->get(ch);
                                tt = GEQ;
                            }
                            tt = LT;
                            break;
                        case '&':
                            //two char ops are strange
                            lexeme.push_back(ch);
                            ch = in->peek();
                            if(ch == '&') {
                                lexeme.push_back(ch);
                                in->get(ch);
                                tt = LOGICAND;
                            }
                            tt = LT;
                            break;
                        case '|':
                            //two char ops are strange
                            lexeme.push_back(ch);
                            ch = in->peek();
                            if(ch == '|') {
                                lexeme.push_back(ch);
                                in->get(ch);
                                tt = LOGICOR;
                            }
                            break;
                        case '(':
                            //two char ops are strange
                                tt = LPAREN;
                                break;
                        case ')':
                            //two char ops are strange
                                tt = RPAREN;
                                break;
                        case ';':
                                tt = SC;
                                break;                 
                    } //end opcase
                    return Token(tt, lexeme, *lineNum);
                    //cant return in case statement
                    
                } //Next state determined by this point
                break;
            //identifiers
            case INID:
                if(isalpha(ch) || isdigit(ch)) {
                    lexeme += ch;
                    break;
                } else {
                    if(ch == '\n') {
                        (*lineNum)--; //have to -1 incase we print
                                     // an err on this lexeme. The next 
                                     // iteration will handle linenum for me.
                    }
                    in->putback(ch);
                    return identificator(lexeme, *lineNum);
                } //identifies keywords

            case ININT:
                if(isdigit(ch)) {
                    lexeme += ch;
                    break;
                } else if(isalpha(ch)) {
                    lexeme += ch;
                    return Token(ERR, lexeme, *lineNum);
                } else {
                    if(ch == '\n') {
                        (*lineNum)--; //it saw the \n so roll bk
                    }
                    in->putback(ch);
                    return Token(ICONST, lexeme, *lineNum);
                }
            case INSTRING:
                lexeme += ch;
                if(ch == '"') {
                    //removes the \"
                    lexeme = lexeme.substr(1, lexeme.length()-2);
                    return Token(SCONST, lexeme, *lineNum);
                }
                if(ch == '\n') {
                    return Token(ERR, lexeme, *lineNum);
                }               
                break; //start the case over
            case INCOMMENT:
                if(ch == '\n') {
                    state = BEGIN;
                }
                break;
        }
    }
    
    if( in->eof() ) //streams end with `eof`
        return Token(DONE, "", *lineNum);
    
    return Token(ERR, lexeme, *lineNum);
}
