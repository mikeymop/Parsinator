#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <ctype.h>

using namespace std;

/**
S = skip the argument and print nothing
L = convert every uppercase letter in the arg to lower print on line by itself
U = convert every lower case letter to upper print on line by itself
r = (lowercase r), print the letters in reverse order.
R = print the letters in reverse and convert lower letters to upper.
else print FAIL

**/
int main(int argc, char* argv[]) {

    int arg;

    //make sure there are args
    if (argc < 2) {
        cout << "There are no args" << endl;
    }

    //traverse argv
    for(int i = 1; i < argc; i++) {
        std::string argument = argv[i];
        char letter = argument[0];
        //debug
        //cout << "letter is: " << letter << endl;
        //cout << "argument is: " << argument <<endl;
        switch (letter)
        {
            case 'S':
                //cout << "Skip and do nothing" << endl;
                break;
            case 'L':
                //Finished: convert string to lower
                argument.erase(0,1);
                transform(argument.begin(), argument.end(),
                    argument.begin(), ::tolower);
                cout << argument << endl;
                break;

            case 'U':
                //Finished: convert to upper
                argument.erase(0,1);
                transform(argument.begin(), argument.end(),
                    argument.begin(), ::toupper);
                cout << argument << endl;
                break;

            case 'r':
                //Finished: print letters in reverse
                argument.erase(0,1);
                reverse(begin(argument),end(argument));
                cout << argument << endl;
                break;

            case'R':
                argument.erase(0,1);
                transform(argument.begin(),argument.end(), argument.begin(),::toupper);
                reverse(begin(argument),end(argument));
                cout << argument << endl;
                break;

            default:
                cout << "FAIL" << endl;
        }
    }
}