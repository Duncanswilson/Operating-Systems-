#include <iostream>
#include <fstream> 
#include <iostream> 
using namespace std; 

int main(int argc, char *argv[]) {
 for( int i = 1; i <= argc; i++)
 {
  ifstream fin(argv[i]); 
  if(!fin.is_open()) 
   cout << "Couldn't open file"; 
  else {
   char x[50]; int y = 0; 
   while(fin.good()) 
    { 
     fin.get(x[y]); 
     fin.ignore(); 
     i++; 
    }
   }
 }
}

