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

std::map<std::string, Value> evars;

void ParseOut(ParseTree *prog);

void RunTimeError(string msg) {
    cout << "0: RUNTIME ERROR " << msg << endl;
    exit(1);
}

int main(int argc, char *argv[]) {
    ifstream ifile;
    istream *in;
    int linenum = 0;
    string arg1 = "";

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
    
    prog->Eval();
    return 0;

    //ParseOut(prog);
}
