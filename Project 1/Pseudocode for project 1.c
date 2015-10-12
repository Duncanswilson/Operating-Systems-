// struct for family: 
struct{ 
  //char parent (father)
  char father; 
  //char parent (mother) 
  char mother; 
  //char arr childern 
  char childern[10]; 
  //int number of childern
  int childNum; 
} family;  

//dynamically allocate family array
   
  
//initalize file operator 

// loop through file until end 

	//read in father 

	//read in mother 
 
	//read in number of children 
    
 	//loop through the children 

 		//read in each child 

//call parent function 

//end program


// Parent function (generation #) 
  // assign a temp family 
  
  //print mother-father pair
  //indent in response to generation number 

  //Loop over the number of children 
	
  	//fork new process 

  	//if child
		//then call child function(child name)

	//parent process wait 

//Child function (child name) 
	
   // loop through all families to see if child is either father or mother 

	//if it is also a parent 
		//then call parent function (loop index #) 

	//else 
		//print child 	


