Hello and thank you for grading the first part of my operating simulator! 

This is the result of my blood, sweat, and tears this spring break so I wanted to outline my methodolgy here so you cna grade with that in mind. 

I early on made the *possibly ill-fated* choice of writing this in C and wanting to use a queue for the meta-data info. I wrote my own queue and included the currently needed functions as I already have a ton of code for this porject, all of that code can be found in the structs&funcs.h file. 

In regards to the timing that this program does, I was confused about the directions in both the instructions and in the ruberic. I initally created a float that would track the exact amount of (milli)seconds elapsed and that outputs the exact result I hoped for. Alas that is not what the ruberic specifies... so I've revised my code with an updated version of timing. 

Here's a few notes on this: 

--- There is no portable way to measure milliseconds in C (see the second answer here: thehttp://stackoverflow.com/questions/8558625/how-to-get-the-current-time-in-milliseconds-in-c-programming) 
I understand the first answer "may" work, but I got such unrealiable times when I implemented a test version with cycle time I ended up going with getTimeofDay()

--- Also the ruberic states: "Correct and appropriate use of system clock to report OS simulation times; each operational time
(e.g., processing, I/O, etc.) is reasonably close relative to the given cycle time and msec per cycle data"

System time is defined as "In computer science and computer programming, system time represents a computer system's notion of the passing of time. In this sense, time also includes the passing of days on the calendar." (from http://en.wikipedia.org/wiki/System_time)

So with that I feel justified in my choice of getTimeofDay, even though it gives imperfeect results. 

But I'm a depressive realist and assume you will grade harshly so I've included a copy of my floating point timer in order for you to test my slightly off times against. 
This can be run by uncommenting the commented out line in the make file and re-run make

Thanks for reading all of this and for grading my program!   