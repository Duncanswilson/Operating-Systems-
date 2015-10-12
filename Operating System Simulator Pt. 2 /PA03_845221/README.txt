Hello and thank you for grading the second part of my operating simulator! 

This is the result of my blood, sweat, and tears this spring break and subsequent weeks so I wanted to outline my methodology here so you can grade with that in mind. 

I have many data structures for wrapping the various and disparate data types together. The main one is the process queue which holds each PCB. It is a priority Queue which if in FIFO orders the processes by the time they arrive. (each process is marked with the same priority, as you'll see in the code) 
 
In each PCB there is a task queue which holds all of the tasks which the process is to run. (you can think of these as instructions stored in registers) 
There also is a thread structure created so that it could be passed to the thread function without much hassle. 

------------------------------------------------------------------------------------------
Timing: 

As you may notice from running my program that the timing outputs are not as clean as the example given... This is a problem I have yet to solve. 

I can produce clean and good looking output like the example if I use a float to keep track of the elapsed time. 
But as soon as I get use the system clock, the microsecond discrepancies start to add up and make it hard to tell if I'm doing anything meaningful at all... 

Here's a few notes on this: 

--- There is no portable way to measure milliseconds in C (see the second answer here: http://stackoverflow.com/questions/8558625/how-to-get-the-current-time-in-milliseconds-in-c-programming) 

--- Also the rubric states: "Correct and appropriate use of system clock to report OS simulation times; each operational time (e.g., processing, I/O, etc.) is reasonably close relative to the given cycle time and msec per cycle data"

If you know of a way to get a better output in C please mark me down, but please let me in on the secret of how to clean this up!!!
--------------------------------------------------------------------------------------------------

But I'm a depressive realist and assume you will grade harshly so I've included a copy of my program which doesn't use the system clock and rather uses a floating point timer in order for you to test my slightly off times against perfect results.
 
This can be run by the command: make time

This command will generate an executable file named floatTime, so you won't have to re-make my program in order to grade each test.   

Thanks for reading all of this and for grading my program!   

PS: Reading this code in SublimeText is much better than gedit :)  
