#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include "functions.h"


int main(int argc, char *argv[])
{
   // create file
   FILE *fin;
   // retrieve file name from the command line argument
   fin = fopen(argv[1], "r");
   // check if file exists, if not move to next or end 
   if(fin == NULL){
      printf("%s\n", "File entered didn't seem to exist, quiting program."); 
   }
    
   else{

	 // get number of families
	 familiesNum = getNumFamilies(fin);
	
	 // go back to beginning of file
	 rewind(fin); 

	 // if no families in file, end program
	 if( familiesNum == 0){
		 printf("%s\n","File doesn't seem to contain any families, quiting program.");
	   	 return 0;
			}

	 // allocate faimlies
	 famArr = (family*)malloc(familiesNum*sizeof(family));

	 //read in from file
	 readIn(fin); 
     
	 // close the file! 
	 fclose(fin);  

	 // call parent function
	 generation = 0; 
	 parent(0);
     }    
   	// return success
	return 0;

}