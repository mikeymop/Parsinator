#include <iostream>
#include <fstream>
#include <ctype.h>
#include <string.h>
#include <algorithm>
#include <map>
#include <vector>
#include <deque>

using namespace std;

/*
-find the most common expressions of length n
across all input files. N is argv[1]
-the mode all | top will be argv[2]
-finall argv[3+] will be the names of all input files
*/

void grabPhrase(int n, std::deque<std::string>& v, std::map<std::string, int> &m);

int main(int argc, char* argv[]) {

    string mode;
    //Finished: handle errs
    if(argc <= 1) {
        cout << "NO PHRASE LENGTH" << endl;
        return -1;
    }
    //Finished: get params from argv[1,2]
    if(isdigit(atoi(argv[1])) || atoi(argv[1]) <= 0) { //is !number
        cout << "INVALID PHRASE LENGTH" << endl;
        return -2;
    }

    int n = atoi(argv[1]);
    mode = argv[2];
    //else check for invalid mode
    if(mode != "all" && mode != "top") {
        cout << "INVALID MODE" << endl;
        return -3;
    } //check for no files
    else if(argc == 3) {
        cout << "NO FILES GIVEN" << endl;
    }
    //for files in argv[3+]
    for(int i = 3; i <= argc-1; i++) {
        ifstream iFile;
        string fname = argv[i];
        iFile.open(fname, ios::in);
        //debug cout << "opening: " << fname << endl;
        //check if valid file
        if(!iFile.is_open()) {
            cerr << "BAD FILE " << fname << endl;7
        }

        if(iFile) {
            //setup
            std::deque<std::string> fileContents;
            std::map<std::string, int> wordmap; //contain all these words
            std::string word; //contain a word
            wordmap.clear();

            //create temp vector
            while(iFile >> word) {
                    //word tolower
                    std::transform(word.begin(), word.end(), word.begin(), ::tolower);
                    fileContents.push_back(word);
            }
            iFile.close();
            //if we have enough words, take the first five
            //commit the five too
            while(fileContents.size() > n) {
                //take `n` words out of `filecontents` and commit them to `wordmap`
                grabPhrase(n, fileContents, wordmap);
                if(fileContents.size() > n) {
                    fileContents.pop_front();
                }
            }
            grabPhrase(n, fileContents, wordmap);
            //TODO: ALL EQUALLY FREQUENT: check if all values the same
            // see: https://stackoverflow.com/questions/20287095/checking-if-all-elements-of-a-vector-are-equal-in-c
            bool equallyFrequent = true;
            for(std::map<std::string,int>::iterator it=wordmap.begin(); it!=wordmap.end(); ++it) {
                if(wordmap.begin()->second != it->second) {
                    equallyFrequent = false;
                    break;
                }
            }
            //EMPTY PHRASES: check for empty map
            bool empty;
            if (wordmap.empty()) {
                empty = true;
                cout << "NO PHRASES" << endl;
            }
            if (equallyFrequent && !empty && wordmap.size() > 1) {
                std::cout << "ALL PHRASES EQUALLY FREQUENT" << endl;
                return -4;
            }

            //done: mode all
            if(mode == "all") {
                for(std::map<std::string,int>::iterator it=wordmap.begin(); it!=wordmap.end(); ++it) {
                        if(it!=wordmap.end()) {
                        std::cout << it->first << " " << it->second << endl;
                        } else {
                            std::cout << it->first << " " << it->second;
                        }
                    }
            }
            if(mode == "top") {
                std::map<std::string, int> maxmap;
                int curVal, maxVal;
                std::string curPhrase, maxPhrase;
                curVal = 0;
                maxVal = 0;
                curPhrase = "";
                maxPhrase = "";

                //find max
                for(std::map<std::string,int>::iterator it=wordmap.begin(); it!=wordmap.end(); ++it) {
                    curPhrase = it->first;
                    curVal = it->second;
                    if(curVal > maxVal) {
                        maxPhrase = curPhrase;
                        maxVal = curVal;
                    }
                }
                //we have FIRST max, find peers
                for(std::map<std::string,int>::iterator it=wordmap.begin(); it!=wordmap.end(); ++it) {
                    curPhrase = it->first;
                    curVal = it->second;
                    maxmap[maxPhrase] = maxVal; //prevent dupes by putting it here
                }
                //finally print maxmap
                for(std::map<std::string,int>::iterator it=maxmap.begin(); it!=maxmap.end(); ++it) {
                    if(it!=wordmap.end()) {
                    std::cout << it->first << " " << it->second << endl;
                    } else {
                        std::cout << it->first << " " << it->second;
                    }
                }
            }
        }
    }
}

void grabPhrase(int n, std::deque<std::string>& v, std::map<std::string, int> &m) {
    //n = phrase size, v = input vector, m = map to commit too
    std::string cPhrase;
    std::string word;
    unsigned int x = v.size();
    if(x >= n) {
        for(int i=0; i < n; i++) {
            //int a = i-1;
            cPhrase.append(v[i]);
            if(i < n-1) {
                cPhrase.append(" ");
            }
        }
        m[cPhrase]++;
    }
}