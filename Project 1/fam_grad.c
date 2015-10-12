#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// data strucutre for each family 
typedef struct
{
	char father;
	char mother;

	char children[10];
	int childCount;
}family;


int generation = 0;
int numfams;
family *fams = NULL;
void parent(int gen);
void child(char name);
int getNumFams(FILE *fin);
int isParent(char child);


int main(int argc, char *argv[])
{
   // create file
   FILE *fin;
   int i; 
   // loop through all files in the command line argument
   //for(i = 1; i < argc; i++)
    //{
    	printf("%d %s\n", i, argv[2]);  
	    fin = fopen(argv[i], "r");
        // check if file exists, if not move to next or end 
        if(fin == NULL){
          printf("%s\n", "File entered did not exist, moving on to next file."); 
        }
	else{
		// get number of families
		numfams = getNumFams(fin);
		// go to beginning of file
		rewind(fin); 
		// if no families in file, end program
		if( numfams == 0){
		   printf("%s\n","The are no faimlies in the file. Moving onto next file.");
		   return 0;
		}

		// allocate faimlies
		fams = (family*)malloc(numfams*sizeof(family));
		//declare index
		int j;
		// prime the loop by checking for EOF
		char primer = fgetc(fin);

		// while not at end of file
		while(primer != -1){	
			//printf("%c\n", temp);
			// get father from file stream
			fams[generation].father = primer;
			// get space
			fgetc(fin);
			// get mother from file stream
			fams[generation].mother = fgetc(fin);
			// get space
			fgetc(fin);
			// get child count from file stream & convert to int 
			fams[generation].childCount = fgetc(fin) - '0';
			// get space
			fgetc(fin);
			// loop for child count
			for(i = 0; i < fams[generation].childCount; i++)
			{
			  // get child from file stream
			  fams[generation].children[i] = fgetc(fin);
			  // get space
			  fgetc(fin);
			} 
			// increase generation
			generation++;
			primer = fgetc(fin);
	    }
	    // close the file! 
	    fclose(fin);  
	    // call parent function
	    generation = 0; 
	    parent(0);
     }    
    //}
   	// return success
	return 0;

}

//////////////////////////////////////////////////////////////////////////
// Functions// 
//////////////////////////////////////////////////////////////////////////

void parent(int gen){
        //initialize process id number and child boolean (int) 
	pid_t pid;
	int childFlag = 1; 

	// create temporary family
	//family tempFam fams[gen];
	
	// loop over tabbing for generation
	int i;
	for(i = 0; i < generation; i++){
	   printf("\t");
	}
	// print parents
	printf("%c%c%c \n", fams[gen].father, '-' ,fams[gen].mother);
	// loop through each child
	for(i = 0; (i < fams[gen].childCount) &&  (childFlag == 1); i++){
	    pid = fork();
            // child process
            if(pid == 0){ 

               //increment the generation counter
	       generation++;
               //set the child flag to stop the loop 
               childFlag = 0; 
               // call the child function 
               child(fams[gen].children[i]);
	     }
            //parent process 	
            else if(pid > 0){
		    // parent process wait
		    wait(NULL);//switch to wait pid
		}
	}
}

void child(char name){
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
	   printf("%c\n", name);
	}
}

int getNumFams(FILE *fin){
        //initialize line counter and buffer  
	int numFams = 0;
	char buffer;
        //loop through file checking for endlines 
	while(fgetc(fin) != -1){
	    buffer = fgetc(fin);
            //if endline is seen increment family number 
	    if(buffer == '\n')
	       numFams++;
	}
       return numFams;
}

int isParent(char child){
	// check child name against parents in family array
	// initialize variable
	int i;
        //Loop through all families 
	for(i = 0; i < numfams; i++)
          {
	   // check with father
	   if(fams[i].father == child)
		return i;	
           // check with mother
	   if(fams[i].mother == child)
		return i;
	 }
	return -1;
}
