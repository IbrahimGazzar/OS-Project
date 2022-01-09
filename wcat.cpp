#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

int main(int argc, char**argv){ 
 for(int i=1;i<argc;i++){
  ifstream in(argv[i]);
  if(!in.is_open()){
   cout<<"wcat: cannot open file"<<endl;
   exit(1);
  }
  else
   cout<<in.rdbuf();
  
 }
 return 0;
}
