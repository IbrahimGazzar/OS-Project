#include <iostream>
#include <fstream>
#include <cstdlib>
#include <bits/stdc++.h>
using namespace std;


int main(int argc, char**argv){ 
 int cnt=0;
 if(argc==1){
  cout<<"wzip: file1 [file2 ...]"<<endl;
  exit(1);
 }
 char x,endx='\0';
 for(int i=1;i<argc;i++){
  ifstream in(argv[i]);
  if(!in.is_open()){
   cout<<"wzip: cannot open file"<<endl;
   exit(1);
  }
  while(!in.eof()){
   string word;
   getline(in,word);
   for(int i=0;word[i]!='\0';i++){
    if(i==0 && endx=='\0'){
     x=word[i];
     cnt++;
    }
    else{
     if(word[i]==x)
      cnt++;
     else{

     fwrite(&cnt,1,sizeof(cnt),stdout);
     fwrite(&x,1,sizeof(x),stdout);
      cnt=1; 
      x=word[i];  
     }
    }
   }
   endx=x;
   if(!in.eof()){
    char sp;
    cnt=0;
    do{
     sp=x;
     in.get(x);
     cnt++;
    }while(x=='\n');
    fwrite(&cnt,1,sizeof(cnt),stdout);
    fwrite(&sp,1,sizeof(sp),stdout);
    cnt=0;
    if(!in.eof()){
     cnt++;
     endx=x;
    }
    else
     endx='\0';
   }
  }
  in.close();
  
 }
 if(cnt!=0){
  fwrite(&cnt,1,sizeof(cnt),stdout);
  fwrite(&x,1,sizeof(x),stdout);
  cnt=0;
 }
 return 0;
}
