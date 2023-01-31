#include <stdio.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <utmp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
//TODO: Make sure to document that values are in Base 10

//Prints the header, will be called regardless of the arguments afterwards. Returns the memory usage of the function in case it's needed in other print statements
int print_header(int sample_amt, int tick_time)
{
	//Recieve the required data from libraries
	struct rusage *pointer_curr_proc = malloc(sizeof(struct rusage));
	getrusage(RUSAGE_SELF, pointer_curr_proc);
	long mem_usage = (pointer_curr_proc->ru_maxrss);
	//Print required statements
    printf("Number of Samples: %d -- every %d seconds\n", sample_amt, tick_time);
    printf("Memory usage: %d kilobytes\n", mem_usage);
    printf("_____________________________________________________\n");
	free(pointer_curr_proc);
    return(mem_usage);
}



//Prints system use in the case that --system is called or no limiting arguments are called
void print_system_use()
{
	//Header
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
}



//Prints one line of the hardware data in one sample (called only in the for loop of print_system_use)
void print_system_samples()
{
    struct sysinfo *pointer = malloc(sizeof(sysinfo));
    sysinfo(pointer);
    float total_physical_mem = (pointer->totalram) / 1000000000 ;
    float used_phy_mem = total_physical_mem - ((pointer->freeram) / 1000000000);
    float total_virt_mem = total_physical_mem + ((pointer->totalswap) / 1000000000);
    float used_virt_mem = total_virt_mem - ((pointer->freeswap) / 1000000000) + used_virt_mem; 
    printf("%f / %f GB -- %f / %f GB\n", used_phy_mem, total_physical_mem, used_virt_mem, total_virt_mem);
    free(pointer);
}



void print_user_section()
{
	setutent();
    printf("### Sessions/users ###\n");
    for (struct utmp *pointer = malloc(sizeof(struct utmp)); getutent() != NULL; pointer = getutent())
    {
		char *username = pointer->ut_user;
		char *dir = pointer->ut_line;
		char *operation = pointer->ut_host;
        printf("%s          %s  (%s)", username, dir, operation);
    }
	endutent();
}
//Helper function for print_system_ending, which calculates the difference in cpu_use given a string which comes from reading the /proc/stat file
long get_cpu_use(char *cpu_info)
{
	long cpu_use = 0;
	char *leftover;
	//The first 3 numeric values of the first line of /proc/cpuinfo refer to the time taken by normal and nice processes from the user and time taken by processes executing in the kernel, which add up to all the time taken by processes total
	cpu_use += strtol(cpu_info, &leftover, 10);
	cpu_use += strtol(leftover, &leftover, 10);
	cpu_use += strtol(leftover, &leftover, 10);
	//Idle process line, which does not contribute to our total
	strtol(leftover, &leftover, 10);
	cpu_use += strtol(leftover, &leftover, 10);
	cpu_use += strtol(leftover, &leftover, 10);
	cpu_use += strtol(leftover, &leftover, 10);
	return cpu_use;
}

//System ending requires us to print the cpu use and the number of CPUs total. Accepts a long that is the CPU use of the previous check iteration. In case old_cpu_use == 0, then there has been no previous check.
//Returns the current cpu use in case of needing that value to recur the next iteration
long print_system_ending(long old_cpu_use)
{
	//Initialize our variables to read from the file
	char new_line[4];
	int core_amt = 0;
	char *cpu_total;
	//Open the file for reading
	FILE *file = fopen("/proc/cpuinfo", "r");
	//Get the first line, which holds the cpu use data
	fgets(cpu_total, 255, file);
	long cpu_use = get_cpu_use(cpu_total);
	float diff_cpu_use = 0;
	if (old_cpu_use != 0)
	{
		diff_cpu_use = (cpu_use - old_cpu_use) / cpu_use;
	}
	while(fgets(new_line, 3, file) != NULL)
	{
		//Get the first 3 characters for each line after the first. For each cpu, the new line will begin with the text "cpu" so we can list how many cpus there are that way
		if (strcmp(new_line, "cpu") == 0)
		{
			core_amt++;
		}
	}
	fclose(file);
    printf("Number of cores: %d\n", core_amt);
    printf("    Total cpu use: %.4f%%\n", diff_cpu_use);
    printf("____________________________\n");
	return(cpu_use);
}


void print_system_info()
{
    struct utsname *pointer = malloc(sizeof(struct utsname));
    uname(pointer);
    printf("### System Information ###\n");
    printf("System name: %s\n", pointer->sysname);
    printf("Machine Name: %s\n", pointer->nodename);
    printf("Version: %s\n", pointer->version);
    printf("Release: %s\n", pointer->release);
    printf("Architecture: %s\n", pointer->machine);
    printf("___________________________\n");
	free(pointer);
}


void print_sequential(int samples, int tick_time, int ex_code)
{
	int process_mem = print_header(samples, tick_time);
    //Code for printing everything
	long cpu_use = 0;
	print_header(samples, tick_time);
    if(ex_code == 0)
    {
		for(int i = 0; i < samples; i++)
        {
            printf(">>>Iteration %d\n", i);
            printf("Memory usage: %d\n", process_mem);
            print_system_use();
            for(int j = 0; j < i; j++)
            {
                printf("\n");
            }
            print_system_samples();
            int rest_of_the_lines = samples - i;
            for(int j = 0; j < rest_of_the_lines; j++)
            {
                printf("\n");
            }
			print_user_section();
            cpu_use = print_system_ending(cpu_use);
			sleep(tick_time);
        }
    }
	//Code for only printing system
	if (ex_code == 1)
	{
        for(int i = 0; i < samples; i++)
        {
            printf(">>>Iteration %d\n", i);
            printf("Memory usage: %d\n", process_mem);
            print_system_use();
            for(int j = 0; j < i; j++)
            {
                printf("\n");
            }
            print_system_samples();
            int rest_of_the_lines = samples - i;
            for(int j = 0; j < rest_of_the_lines; j++)
            {
                printf("\n");
            }
            cpu_use = print_system_ending(cpu_use);
			sleep(tick_time);
        }
    }
    //Code for only printing users
    if (ex_code == 2)
    {
        for(int i = 0; i < samples; i++)
        {
			printf(">>>Iteration %d\n", i);
            printf("Memory usage: %d\n", process_mem);
			print_user_section();
			cpu_use = print_system_ending(cpu_use);
            sleep(tick_time);
        }
    }
	print_system_info();
}

void print_normal(int samples, int tick_time, int exclusion_code)
{
	char *pointer_old_info[samples];
	for (int i = 0; i < samples; i++)
	{
		
	}
}




int main(int argc, char **argv)
{
//Default values for samples and tick times (changes if user inputs some values)
	int samples = 10;
	bool samples_initialized = false;
	int tick_time = 1;
	bool tick_time_initialized = false;
//Defaults for different metrics the user might want,initialized to false and changed to true only if the user explicitly says
	bool system_only = false;
	bool fancy = false;
	bool user_only = false;
	bool sequential = false;
	//This for loop searches all inputs for any modifications to be made to the basic print structure
	for(int i = 0; i < argc; i++)
	{
		if(strcmp(*(argv+i), "--system") == 0)
		{
			system_only = true;
		}
		if(strcmp(*(argv+i), "--user") == 0)
		{
			user_only = true;
		}
		if(strcmp(*(argv+i), "--graphics") == 0)
		{
			fancy = true;
		}
		if(strcmp(*(argv+i), "--sequential") == 0)
		{
			sequential = true;
		}
		if(strlen(*(argv+i)) == 1 && isdigit(**(argv+i)) && !samples_initialized);
		{
			samples = strtol(*(argv+i), NULL, 10);
			samples_initialized = true;
		}
		if(strlen(*(argv+i)) == 1 && isdigit(**(argv+i)) && samples_initialized && !tick_time_initialized)
		{
			tick_time = strtol(*(argv+i), NULL, 10);
			tick_time_initialized = true;
		}
	}
    //Cover the case of user being an idiot and not reading documentation and inputting exclusive arguments
	if (system_only && user_only)
	{
		printf("Not a valid input, --system and --user are exclusive arguments\n");
		return 1;
	}
	//Cover the case of printing only one element
	if (system_only || user_only)
	{
		//An exclusion code allows us to determine which metric to produce in the case of exclusion. If no exclusion, the code is 0
		int exclusion_code;
		if (system_only)
		{
			exclusion_code = 1;
		}
		if (user_only)
		{
			exclusion_code = 2;
		}
		if (sequential)
		{
			print_sequential(samples, tick_time, exclusion_code);
			return 0;
		}
		print_normal(samples, tick_time, exclusion_code);
		return 0;
	}
	//At this point, we know that we must print all metrics
	if (sequential)
	{
		print_sequential(samples, tick_time, 0);
		return 0;
	}
	print_normal(samples, tick_time, 0);
    return 0;
}
//TODO: Implement graphics argument
