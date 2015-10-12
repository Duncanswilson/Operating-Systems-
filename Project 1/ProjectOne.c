#include <stdio.h>

// struct for family: 
struct family{ 
  //char parent (father)
  char father; 
  //char parent (mother) 
  char mother; 
  //char arr childern 
  char childern[10]; 
  //int number of childern
  int childNum; 
};
int familyNum(FILE *f);
//dynamically allocate family array *not dynamic atm* 
family famArr[10]; 


//main    
int main(int argc, char *argv[]) { 

//initalize file operator 
FILE *fin; 
for(int i =1; i<=argc; i++)
{
//Open file 
fin = fopen(argv[i], "r"); 
//Get number of families 
int familyNumber = familyNum(fin);
if(fin)
{ 
// loop through file until end 
   
	//read in father 

	//read in mother 
 
	//read in number of children 
    
 	//loop through the children 

 		//read in each child 

//call parent function 
}
else{
 printf("File couldn't be opened"); 
}
//end program
} 
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
   return lineCounter; 
} 

// Parent function (generation #) 
void parent(int genNum) { 
  // assign a temp family 
  family f = famArr[genNum]; 
  //print mother-father pair
  printf("%c %c %c /n", f.father, ' ', f.mother);
  //indent in response to generation number 

  //Loop over the number of children 
	
  	//fork new process 

  	//if child
		//then call child function(child name)

	//parent process wait 
} 

//Child function (child name) 
	
   // loop through all families to see if child is either father or mother 

	//if it is also a parent 
		//then call parent function (loop index #) 

	//else 
		//print child 	


