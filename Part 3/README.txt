Hello and thank you for grading the third and FINAL part of my operating simulator! 

This is the result of my blood, sweat, and tears this spring break and subsequent weeks so I wanted to outline my methodology here so you can grade with that in mind. 
________________________________________________________________________________________

Data Types: 
I have many data structures for wrapping the various and disparate data types together. The main one is the process queue which holds each PCB. It is a priority Queue which if in FIFO orders the processes by the time they arrive. (By setting the priority to 0 upon reading in, as you can see in the code) 

Task Queue/Stack:  
In each PCB there is a task structure which holds all of the tasks for the process to run. (you can think of these as instructions stored in registers)
The task structure is for readin purposes a queue as that is the best fitting structure for that task, but once you start the simulator I treat it like a stack. 
My reasoning for that is that you need the task info both before and after threads interrupt, so I just push the most recent task back onto the "stack" until 
I/O needs to be done. 
I have left my pop function in, just to indicate when the task structure is being used as a stack. (even though there is not a difference between it and dequeue)

ThreadStuct and Interrupt handling: 
There also is a thread structure created so that it could be passed to the thread function without much hassle. In this structure is a pointer to the interrupt buffer. This interrupt buffer is an int which each thread can communicate to the simulator via changing the value from zero to whatever the PID is of the interrupting process. This way on each iteration of the OS all interrupts will be handled in the order they return. 

Thanks for reading all of this and for grading my program!   

PS: Reading this code in SublimeText is much better than gedit :)  


Old but still useful notes: 
------------------------------------------------------------------------------------------
Timing: 

As you may notice from running my program that the timing outputs are not as clean as the example given... This is a problem I have yet to solve. 

I can produce clean and good looking output like the example if I use a float to keep track of the elapsed time. 
But as soon as I get use the system clock, the microsecond discrepancies start to add up and make it hard to tell if I'm doing anything meaningful at all... 

Here's a few notes on this: 

--- There is no portable way to measure milliseconds in C (see the second answer here: http://stackoverflow.com/questions/8558625/how-to-get-the-current-time-in-milliseconds-in-c-programming) 

--- Also the rubric states: "Correct and appropriate use of system clock to report OS simulation times; each operational time (e.g., processing, I/O, etc.) is reasonably close relative to the given cycle time and msec per cycle data"

If you know of a way to get a better output in C please mark me down, but please let me in on the secret of how to clean this up!!!
-------------------------------------------------------------------------------------------

