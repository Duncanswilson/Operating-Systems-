#include "structs-and-vars.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////////
// Function Headers // 
//////////////////////////////////////////////////////////////////////////

// File Functions // 
int getNumFamilies(FILE *fin);
void readIn(FILE *fin);
void parseFunction(char *familyString);

// Process & Printing Functions // 
void parent(int gen);
void child(char *name, pid_t pid);
int isParent(char *child);



//////////////////////////////////////////////////////////////////////////
// Function Implementation // 
//////////////////////////////////////////////////////////////////////////

//Read through the number of families in the file//
int getNumFamilies(FILE *fin){
        //initialize line counter and buffer  
	int numFamilies = 0;
	char buffer;
        //loop through file checking for endlines 
	while(!feof(fin)){
	    buffer = fgetc(fin);
            //if endline is seen increment family number 
	    if(buffer == '\n')
	       numFamilies++;
	}
       return numFamilies;
}

//Read in file while storing each line appropiately in the data structure// 
void readIn(FILE *fin){
	//Declare the variables for the getline function
    char *lineData = NULL;
    size_t length = 0;
    ssize_t readData; 

    //Loop through the file line by line 
    while((readData = getline(&lineData, &length, fin)) != EOF){
    	//Pass the line to the parse function to be stored in the data structure 
    	parseFunction(lineData); 
    	//Incriment the generation per line 
    	generation++; 
    }

}


void parent(int gen){
    //initialize process id number and child boolean (int) 
	pid_t pid;
	int childFlag = 1; 
	
	// loop over tabbing for generation
	int i;
	for(i = 0; i < generation; i++){
	   printf("\t");
	}
	// print parents
	printf("%s (%i)-%s \n", famArr[gen].father, getppid(),famArr[gen].mother);
	// loop through each child
	for(i = 0; (i < famArr[gen].childCount) && (childFlag == 1); i++){
	    pid = fork();
            // child process
            if(pid == 0){ 
               //increment the generation counter
	           generation++;
               //set the child flag to stop the loop 
               childFlag = 0; 
               // call the child function 
               child(famArr[gen].children[i],getppid());
	     }
            //parent process 	
            else if(pid > 0){
		    // parent process wait
		    waitpid(pid, NULL, 0);//switch to wait pid
		}
	}
}

//
void child(char *name, pid_t pid){
	// check to see the name is a parent
	int parentGen = isParent(name);

	// if name is a parent, call the parent function
	if(parentGen >= 0){
	   parent(parentGen);
         }
	// if not parent, then print name based on generation
	else{	
	   //loop to print tabs, 
           //delimited by the global var generation 
	   int i;
	   for(i = 0; i < generation; i++){
	      printf("\t");
	   }
	   // print child name
	   printf("%s(%i)\n", name, pid);
	}
}

//Check child name against all parents in family array//
int isParent(char *child){
	// initialize iterator variable
	int i;
     //Loop through all families 
	for(i = 0; i < familiesNum; i++){
	   // Comapre against father's name
	   if(strcmp(famArr[i].father,child) == 0)
		return i;	
	 }
	return -1;
}

void parseFunction(char *familyString){
	//declare indexing variables 
	int glIndex = 0;
    int nameIndex = 0; 
    int childCounter = 0;
    //assign temp family for current generation
    family tempFam = famArr[generation];
	//read in the first name of file (father) 
	 while(familyString[glIndex] != ' '){
	  //copy characters into father C-style string 
        tempFam.father[nameIndex] = familyString[glIndex]; 
	  // update index for father name 
        nameIndex++;
	  //update family string index 
        glIndex++;
      }
    //reset name index 
      nameIndex = 0; 
    // move over space 
      glIndex++; 
	//read in second name in file  (mother) 
	 while((familyString[glIndex] != ' ') && (familyString[glIndex] != '\n')){
	   //copy characters into mother C-style string
       tempFam.mother[nameIndex] = familyString[glIndex]; 
	   // update index for mother name 
       nameIndex++;
	   //update family string index 
	   glIndex++;
     }
	//loop through until end of the line (newline character)
	 while(familyString[glIndex] != '\n'){   
	    //reset name index 
	      nameIndex = 0; 
	    // move over space 
	      glIndex++;  
		//loop through for each child stopping on spaces or end of line (newline char) 
	     while((familyString[glIndex]!= ' ') && (familyString[glIndex] != '\n')){
	        //copy character to the child C-style string 
	        tempFam.children[childCounter][nameIndex] = familyString[glIndex]; 
	        // update index for child name 
	        nameIndex++;
	        //update family string index 
	        glIndex++;
	     }
	    //update the index for children
	    childCounter++; 
    }
	//add the final child count to the data structure     
	tempFam.childCount = childCounter;
	famArr[generation] = tempFam;
}
