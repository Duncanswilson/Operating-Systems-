#include <stdio.h> 
int familyNum(FILE *f);

int main(int argc, char *argv[] ) 
{
FILE *fin; 
fin = fopen(argv[1], "r"); 
FILE *finCopy = fin; 
int familyNumber = familyNum(fin); 
char ch; 
ch = fgetc(finCopy);
printf("%c \n", ch); 
return 0; 
}

//Int familyNum function (file operator) 
int familyNum(FILE *f) { 
    int lineCounter = 0;
    char ch; 
   //loop until end of file 
    while(!feof(f)){
      ch = fgetc(f);
      //count the number of lines in the file  
      if(ch == '\n')
      lineCounter++;
    }
   //return count 
   return lineCounter-1; 
} 

