#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <map>
#include <vector>
 
using namespace std;
 
int main(int argc, char *argv[])
{
    
    if (argc == 1){
        cout << "NO PHRASE LENGTH" << endl;
        return 0;
    }
    else if (!atoi(argv[1]) || argv[1] == 0){
        cout << "INVALID PHRASE LENGTH" << endl;
        return 0;
    }
    else if (argc == 2){
        cout << "NO MODE"<<endl;
        return 0;
    }
    else if (strcmp(argv[2], "all") != 0 and strcmp(argv[2], "top") != 0){
        cout << "INVALID MODE" << endl;
        return 0;
    }
    else if (argv[3] == 0){
        cout << "NO FILES GIVEN" << endl;
        return 0;
    }
    for (int i = 3; i < argc; i++){
        ifstream f;
        f.open(argv[i]);
        if (!f.is_open())
        {
            cout << "BAD FILE " << argv[i] << endl;
            return 0;
        }
        
        string words;
        stringstream characters;
        locale loc;
        
        map<string, int> counter;
        map<int, vector<string>> mapv;
        map<string, int>::iterator iter;
       
        int length = strtol(argv[1], nullptr, 0);
        int count = 0;
        // goes through file and ensures count is < length
        while (f >> words && count < length)
        {
            string lower;
            // turns characters into lower case
            for (char character : words){
                lower += tolower(character, loc);
            }
            characters << lower << " ";
            count++;
            //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            // if count reaches the end
            if (count != length)
            {
                string phrase = characters.str();
                
                if (counter.find(phrase) == counter.end()){
                   counter[phrase]++;
                   count++;
                }else{
                   counter[phrase]++;
                }
                characters.str("");
                count = 0;
            } //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        }
        // if map in the file is empty
        if (counter.empty())
        {
            cout << "NO PHRASES" << endl;
            return 0;
        }
        // word is phrase
        string word(argv[2]);    
        
        // iterates through map, for counter, (esentially a list) returns key = value
        for(iter = counter.begin(); iter!= counter.end(); iter++){
            mapv[iter->second].push_back(iter->first);
        }

        // If same frequency then, print ALL Phrases are Frequent
        if(mapv.size() < 2){//check vector string
                cout << "ALL PHRASES EQUALLY FREQUENT" << endl;
                return 0;
        }       
        
                
        if (word == "all")
        {
            // just print all the phrases and show the value
            for (iter = counter.begin(); iter != counter.end(); iter++){
                cout << iter->first << iter->second << endl;
            }
        }
        // If phrase is top frequency
        else if (word == "top")
        {
            for(auto iter = mapv.begin(); iter!=mapv.end();iter++){
                //format:
                //loop thru vec at iter->second
                //print string in vector
                //print space
                //print num at iter->first
                //new line
                for(int i = 0; i < iter->second.size();i++){
                    cout << iter->second[i] << " " << iter->first << endl;
                }
                break;
            }
        }
        
    } 
    return 0;
}