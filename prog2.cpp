/* Michael DeFrancesco November 2018 */
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <algorithm>
#include "tokens.h"

using namespace std;

/*
return Token(token type, lexeme, line#)
return Token(Done, "", lineNum)

-v every token is printed
-sum if present, summary information is printed
-allids a list of the lexemes for all identifiers printed in
    alphabetical order
filename read from the filename; otherwise read from standard in
*/

int main(int argc, char* argv[]) {
    //args
    //int numfiles;
    bool isfile = false;
    bool v = false;
    bool sum = false;
    bool allids;

    int lineNum  = 0;
    int stringCt = 0;
    int identCt  = 0;
    int tokenCt  = 0;
    
    istream *in = &cin;
    ifstream iFile;

    std::vector<std::string> idents;

    string arg1;
    string source;
    
    Token tok;

    for(int i=1; i < argc; i++) {
        //numfiles = 0;
        arg1 = argv[i];
        //Done: arg tester

        if(arg1[0] == '-') {
            if(arg1 == "-v") {
                //do -v
                v = true;
            } else if(arg1 == "-sum") {
                //do sum
                sum = true;
            } else if(arg1 == "-allids") {
                //do allids
                allids = true;
            } else {
                cout << "INVALID FLAG " << arg1 << endl;
                return 2;
            }
        } else {
            //arg must be a filename test it
            if( isfile ) {
                cout << "TOO MANY FILE NAMES" << endl;
                return 3;
            }
            isfile = true;
            source = argv[i];   
        }
        
        //handle files if you have them2
        if(isfile) {
            iFile.open(source);
            if(!iFile.is_open()) {
                cerr << "UNABLE TO OPEN " << source << endl;
                return 3;
            } 
            
            in = &iFile;
        }
    }
        
                while((tok = getNextToken(in, &lineNum)) != ERR && tok != DONE) {
                // handle verbose mode
                    if(v) {
                        cout << tok << endl;
                    }
                    if(allids && tok.GetTokenType() == IDENT) {
                        idents.push_back(tok.GetLexeme());
                    }
                    if(sum) {
                        tokenCt++;
                        if(tok.GetTokenType() == SCONST) { stringCt++; }
                        if(tok.GetTokenType() == IDENT) { identCt++; }
                    }
                } //while
                if(tok.GetTokenType() == ERR) {
                    cout << "Error on line " << lineNum << " (";
                    cout << tok.GetLexeme() << ")" << endl;
                }
                if(allids) {
                    //need alphabetical order
                    sort(idents.begin(), idents.end());
                    //have to remove dupes
                    idents.erase(unique(idents.begin(), idents.end()), idents.end());
                    cout << "IDENTIFIERS: ";
                    for (int i = 0; i <= idents.size() - 1; ++i) {
                        if(i == idents.size()-1) {
                            cout << idents[i] << endl;
                        } else {
                            cout << idents[i] << ", ";
                        }
                    }
                }
                //sum stuff
                if(sum) {
                    cout << "Total lines: " << lineNum << endl;
                    cout << "Total tokens: " << tokenCt << endl;
                    cout << "Total identifiers: " << identCt << endl;
                    cout << "Total strings: " << stringCt << endl;

                }

}
