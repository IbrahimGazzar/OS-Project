#include <bits/stdc++.h>
#include <cstdio>
#include <fstream>
using namespace std;

int main(int argc, char **arg){
    if (argc == 1){
        cout << "wgrep: searchterm [file ...]" << endl;
        return 1;
    }
    FILE *in = freopen(arg[2], "r", stdin);
    if (argc == 3){
        if (in == NULL){
            cout << "wgrep: cannot open file" << endl;
            return 1;
        }
    }
    string key = arg[1];
    string line;
    char c;
    bool check=0;
    while (c != EOF){
        if (c == '\n'){
            c = getchar();
            continue;
        }
        getline(cin, line);
        line = c + line;
        if (line.find(key) != string::npos){
            line.erase(line.begin());
            if (check)
                cout << c;
            cout << line << endl;
        }
        c = getchar();
        check=1;
    }
    return 0;
}
