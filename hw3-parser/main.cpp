/*
 * main.cpp
 */

#include "tokens.h"
#include "parse.h"
#include "parsetree.h"
#include "tokens.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;

/*
Usage:
Run with no args. Enter tokens as the return type of the token from lex.cpp
^d to return or SIGQUIT + to exit.
Index:
    Primaries:
        PLUS
        MINUS
        STAR
        SLASH
    Int:
        ICONST([1-.*])
    "String":
        SCONST(.*)
    IDENT:
        IDENT(x)
    Bool:
        TRUE
        FALSE
    print:
        PRINT
    if:
        IF
    =:
        ASSIGN
    ;:
        SC

*/
void ParseOut(ParseTree *prog);

int main(int argc, char *argv[]) {
    ifstream ifile;
    istream *in;
    int linenum = 0;
    string arg1 = "";
    //bool emptyfile = false;
    Token tok;

    // CHECK ARGUMENTS, OPEN FILES
    //numfiles = 0;
    if(argc > 1) {
        arg1 = argv[1];
        ifile.open(arg1);
        if(argc > 2) {
            cout << "TOO MANY FILENAMES" << endl;
            return -1;
        } else if(!ifile.is_open() ) {
            cout << "COULD NOT OPEN " << arg1 << endl;
            return -2;
        } else {
            //dont even need this
            in = &cin;
        }

        in = &ifile;
    } else {
        in = &cin;
    }

    ParseTree *prog = Prog(in, &linenum);

    if( prog == 0 )
        return 0; // quit on error
    
    ParseOut(prog);

    return 0;
}
