#include <stdio.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <utmp.h>
#include <stdbool.h>
#include <stdlib.h>
//TODO: Make sure to document that values are in Base 10

//Prints the header, will be called regardless of the arguments afterwards. Returns the memory usage of the function in case it's needed in other print statements
long print_header(int sample_amt, int tick_time)
{
	//Recieve the required data from libraries
	struct rusage *pointer_curr_proc = malloc(sizeof(rusage));
	getrusage(RUSAGE_SELF, pointer_curr_proc);
	int mem_usage = (pointer_curr_proc->ru_maxrss) / 1000;
	//Print required statements
    printf("Number of Samples: %d -- every %lf seconds\n", sample_amt, tick_time);
    printf("Memory usage: %d\n", mem_usage);
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
    sysinfo *pointer = malloc(sizeof(sysinfo));
    sysinfo(pointer);
    float total_physical_mem = (pointer->total_ram) / 1000000000 ;
    float used_phy_mem = total_physical_mem - ((pointer->free_ram) / 1000000000);
    float total_virt_mem = total_physical_mem + ((pointer->totalswap) / 1000000000);
    float used_virt_mem = total_virt_mem - ((pointer->freeswap) / 1000000000) + used_virt_mem; 
    printf("%lf / %lf GB -- %lf / %lf GB\n", used_phy_mem, total_phy_mem, used_virt_mem, total_virt_mem);
    free(pointer);
}



void print_user_section()
{
    utmp *pointer = malloc(sizeof(utmp))
    utmp
    printf("### Sessions/users ###\n");
    for (int i = 0; i < sample_amt; i++)
    {
        print_user_line();
        getUserInfo()
        printf("%s          %s  (%s)", *username, *dir, *operation);
    }

}


void print_system_ending()
{
    printf("Number of cores: %d\n", core_amt);
    printf("    Total cpu use: %lf%%\n", cpu_use);
    printf("____________________________\n");
}


void print_system_info()
{
    utsname *pointer = malloc(sizeof(utsname));
    uname(pointer);
    printf("### System Information ###\n");
    printf("System name: %s\n", pointer->sysname);
    printf("Machine Name: %s\n", pointer->machine);
    printf("Version: %s\n", pointer->version);
    printf("Release: %s\n", pointer->release);
    printf("Architecture: %s\n", arch_name);
    printf("___________________________\n");
}


void print_sequential(int samples, int tick_time, int ex_code)
{
	long process_mem = print_header(samples, tick_time);
    //Code for printing everything
    if(ex_code == 0)
    {
        print_system_use();
        //TODO: Implement this part
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
            print_system_ending();
        }
        print_system_info();
        sleep(tick_time);
    }
    //Code for only printing users
    if (ex_code == 2)
    {
            //TODO: Implement this part
    }
}
		

int main(int argc, char **argv)
{
//Default values for samples and tick times (changes if user inputs some values)
	int samples = 10;
	int tick_time = 1;
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
		if(strcmp(*(argv+i), "--graphics"))
		{
			fancy = true;
		}
		if(strcmp(*(argv+i), "--sequential"))
		{
			sequential = true;
		}
		if(strncmp(*(argv+i), "--samples=", sizeof(char) * 10))
		{
			samples = strtol(*(argv+i), NULL, 10);
		}
		if(strncmp(*(argv+i), "--tdelay=", sizeof(char) * 9))
		{
			tick_time = strtol(*(argv+i), NULL, 10);
		}
	}
    //Cover the case of user being an idiot and not reading documentation and inputting exclusive arguments
	if (system_only && user_only)
	{
		printf("Not a valid input, --system and --user are exclusive arguments\n")
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
