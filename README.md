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

--graphics [or --g] (Provides additional visual representations of changes in hardware memory and changes in cpu use)

--sequential (Instead of clearing the screen after each print, prints each sample iteration after the next one)

--samples=N (changes the number of samples printed to N, default is 10 if this argument is not specified) [N is always typecasted to an integer]

--tdelay=T (Changes the time between sample prints to T, default is 1 if this argument is not specified) [T is always typecasted to an integer]

Additionally, the client may simply pass integers as arguments, the first integer will be treated as if in the --samples=N arguments, the second
(if there is one) will be treated as if in the tdelay=T argument.

These arguments may be used all together, except for --system and --user. If that is the case then an exception will be returned.

ANALYSIS INFO:

The code was designed modularly, with helper methods to aid readability and debugging.
The following will present all methods, along with what they are helper functions to and their return values.

print_header is a helper function for main. It prints the introduction to the code, including number of samples to print, the amount of time
in between sample prints, and the memory taken for this process. It returns the memory taken, as an integer.

print_system_use prints the header of the system hardware function.

print_system_samples prints an individual line of the hardware data, in gigabytes.

print_user_section prints the user processes, the user who is using these processes, and what those processes are.

get_cpu_use gets the current cpu use and idle use. It is a helper function to print_system_ending, and returns the current cpu use, as well as
changing the value at the pointer of idle.

print_system_ending accepts the last cpu use data, prints out the difference, and returns the current cpu use data in order to be used in a future iteration. Idle is also needed to take out processes the PC is not currently working on.

print_system_info prints the final lines of the statement, like OS, version, release, etc.

print_sequential prints all the required information, using the above methods, in the case that --sequential was passed as an argument by 
the client. It iterates the number of samples required in a for loop, printing each statement and counting the next iteration after a new line.

print_normal prints all the required information in the case --sequential was not passed. It clears the screen after each iteration, and uses the
above methods to print all data after passing them into a for loop.

print_fancy_system_ending_seq is a specific helper function for if the user passes both --sequential and --graphics (or --g) as arguments. It
is a helper function for print_fancy_seq, as extra functionality is required to represent the data in a visual way, like taking the difference 
between the current and previous cpu use and printing is as |.

print_fancy_system_ending_normal follows a similar structure, except this helper function must also remember all previous strings printed, in order
to print them all again after the screen has been cleared, thus necessitating the need for a different helper function for the print_fancy_normal function.

print_fancy_system_samples is a helper function for both print_fancy_seq and print_fancy_normal, and prints the system sample lines with the 
additional # to denote the difference between the last and current iteration.

print_fancy_seq is called by main when --sequential and  --graphics (or --g) are called as arguments by the user. Prints all required info in a 
sequential format using the above helper functions.

print_fancy_normal is called by main when --graphics is called as an argument. Prints all required info normally, clearing the screen after each use,
using the same helper functions as before, except using fancy helper functions when applicable.

take_awguments_UwU is a helper function to main that checks the arguments passed by the user, and setting the values to variables that are accepted
as arguments to the method itself. The actual return value of this method is an error type. If 0, then the client has inputted well-formatted information, otherwise a 1 is returned.

The main function first intitializes values for all the different arguments the user may pass, then parses them using the take_awguments_UwU helper
method.
A check is then made to ensure that all arguments are accepted. main then checks each flag, to see if we need to print only certain info. 
First, it checks if either --system or --user were passed, and if so, prints the current info using an exclusion code. This exclusion code allows
our methods to easily check what info they should be printing, if 0, everything is printed. 1 means only system is printed, 2 means only user.
On all calls, we check to see if the user wants normally printed info, or sequentially printed info, calling the current helper method as needed.
Also, the --graphics arguments is also checked to ensure it was printed or not.
