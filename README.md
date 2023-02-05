First Lab of B09. Checks and prints various system information.

This README file will be divided into 3 parts, giving an introduction, describing client information, then explaining the actual code itself.
If only using this code, only the client information needs to be read.

CLIENT INFO:

What follows is the arguments that can be passed, as well as prerequisites, details, and edge cases.
All numeric information is given in base 10.
System hardware info and cpu use difference are provided to 4 decimal points of accuracy.
The software only runs on Linux devices. The code was tested on Linux 6.1.8.
By default, the program prints 10 samples, refreshing every 1 second. Each sample includes the following, in this order:

-Hardware memory allocated for this program

-Physical and virtual RAM used up compared to the total of the PC

-Systems running and the users of those systems

-PC Hardware information about the number of CPUs and the current CPU use of the PC

-System information, such as Operating System, version, release etc.


The following are optional arguments that may be passed: (different arguments must be seperated by blank space)

--system (Excludes the user section of the printing)

--user (Excludes the system section of the printing)

--graphics (Provides additional visual representations of changes in hardware memory and changes in cpu use)

--sequential (Instead of clearing the screen after each print, prints each sample iteration after the next one)

--samples=N (changes the number of samples printed to N, default is 10 if this argument is not specified) [N is always typecasted to an integer]

--tdelay=T (Changes the time between sample prints to T, default is 1 if this argument is not specified) [T is always typecasted to an integer]

Additionally, the client may simply pass integers as arguments, the first integer will be treated as if in the --samples=N arguments, the second
(if there is one) will be treated as if in the tdelay=T argument.

These arguments may be used all together, except for --system and --user. If that is the case then an exception will be returned.

ANALYSIS INFO:
The code was designed modularly, with helper methods to aid readability and debugging.
The following will present all methods, along with what they are helper functions to and their return values.
