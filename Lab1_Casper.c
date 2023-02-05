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
#include <math.h>
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
    printf("Memory usage: %ld kilobytes\n", mem_usage);
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
    struct sysinfo pointer;
	sysinfo(&pointer);
    float total_physical_mem = ((float)(pointer.totalram)) / 1000000000;
    float used_phy_mem = total_physical_mem - (((float)(pointer.freeram)) / 1000000000);
    float total_virt_mem = total_physical_mem + (((float)(pointer.totalswap)) / 1000000000);
    float used_virt_mem = total_virt_mem - ((float)(pointer.freeswap) / 1000000000) - (((float)(pointer.freeram)) / 1000000000);
    printf("%.4f / %.4f GB -- %.4f / %.4f GB\n", used_phy_mem, total_physical_mem, used_virt_mem, total_virt_mem);
}

void print_user_section()
{
	setutent();
    printf("### Sessions/users ###\n");
	struct utmp *pointer = getutent();
    while(pointer != NULL)
    {
		if (pointer->ut_type == USER_PROCESS)
		{
        printf("%s\t\t%s\t(%s)\n", pointer->ut_user, pointer->ut_line, pointer->ut_host);
		}
		pointer = getutent();
	}
	endutent();
	printf("_______________________________________\n");
}
//Helper function for print_system_ending, which calculates the difference in cpu_use given a string which comes from reading the /proc/stat file
long get_cpu_use(char *cpu_info, long *idle)
{
	long cpu_use = 0;
	char *leftover;
	for(int i = 0; i < 7; i++)
	{
		leftover = memchr(cpu_info, ' ', strlen(cpu_info));
		if (i == 4)
		{
			*(idle) = strtol(leftover, &leftover, 10);
			cpu_use += *idle;
			continue;
		}
		cpu_use += strtol(leftover, &leftover, 10);
	}
	return cpu_use;
}

//System ending requires us to print the cpu use and the number of CPUs total. Accepts a long that is the CPU use of the previous check iteration. In case old_cpu_use == 0, then there has been no previous check.
//Returns the current cpu use in case of needing that value to recur the next iteration
long print_system_ending(long old_cpu_use, long *old_idle)
{
	//Initialize our variables to read from the file
	char new_line[4];
	int core_amt = 0;
	char cpu_total[255];
	//Open the file for reading
	FILE *file = fopen("/proc/stat", "r");
	//Get the first line, which holds the cpu use data
	fgets(cpu_total, 255, file);
	long new_idle;
	long cpu_use = get_cpu_use(cpu_total, &new_idle);
	float print_val = 0;
	if (old_cpu_use != 0)
	{
		int cp_diff = (cpu_use - old_cpu_use);
		int idle_diff = (new_idle - *old_idle);
		int actual_cp = cp_diff - idle_diff;
		print_val = (float)(actual_cp)/((cpu_use + old_cpu_use)-(new_idle + *old_idle));
	}
	char *end_check = fgets(new_line, 4, file);
	while(end_check)
	{
		//Get the first 3 characters for each line after the first. For each cpu, the new line will begin with the text "cpu" so we can list how many cpus there are that way
		if (strcmp(new_line, "cpu") == 0)
		{
			core_amt++;
		}
		end_check = fgets(new_line, 4, file);
	}
	print_val = print_val * 100;
	*old_idle = new_idle; 
	fclose(file);
    printf("Number of cores: %d\n", core_amt);
	printf("\tCPU Usage: %.4f %%\n", print_val);
    //printf("    Total cpu use: %.4f%%\n", diff_cpu_use);
    printf("____________________________\n");
	return(cpu_use);
}


void print_system_info()
{
    struct utsname pointer;
    uname(&pointer);
    printf("### System Information ###\n");
    printf("System name: %s\n", pointer.sysname);
    printf("Machine Name: %s\n", pointer.nodename);
    printf("Version: %s\n", pointer.version);
    printf("Release: %s\n", pointer.release);
    printf("Architecture: %s\n", pointer.machine);
    printf("___________________________\n");
}


void print_sequential(int samples, int tick_time, int ex_code)
{
	int process_mem = print_header(samples, tick_time);
	long cpu_use = 0;
	long idle_proc = 0;
	//Code for printing everything
    if(ex_code == 0)
    {
		for(int i = 0; i < samples; i++)
        {
            printf(">>>Iteration %d\n", i);
            printf("Memory usage: %d kilobytes\n", process_mem);
			printf("____________________________\n");
            print_system_use();
            for(int j = 0; j < i; j++)
            {
                printf("\n");
            }
            print_system_samples();
            int rest_of_the_lines = samples - i;
            for(int j = 1; j < rest_of_the_lines; j++)
            {
                printf("\n");
            }
			printf("____________________________\n");
			print_user_section();
            cpu_use = print_system_ending(cpu_use, &idle_proc);
			sleep(tick_time);
        }
    }
	//Code for only printing system
	if (ex_code == 1)
	{
        for(int i = 0; i < samples; i++)
        {
            printf(">>>Iteration %d\n", i);
            printf("Memory usage: %d kilobytes\n", process_mem);
			printf("____________________________\n");
            print_system_use();
            for(int j = 0; j < i; j++)
            {
                printf("\n");
            }
            print_system_samples();
            int rest_of_the_lines = samples - i;
            for(int j = 1; j < rest_of_the_lines; j++)
            {
                printf("\n");
            }
			printf("____________________________\n");
            cpu_use = print_system_ending(cpu_use, &idle_proc);
			sleep(tick_time);
        }
    }
    //Code for only printing users
    if (ex_code == 2)
    {
        for(int i = 0; i < samples; i++)
        {
			printf(">>>Iteration %d\n", i);
            printf("Memory usage: %d kilobytes\n", process_mem);
			printf("____________________________\n");
			print_user_section();
			cpu_use = print_system_ending(cpu_use, &idle_proc);
            sleep(tick_time);
        }
    }
	print_system_info();
}

void print_normal(int samples, int tick_time, int exclusion_code)
{
	long cpu_use = 0;
	long idle_proc = 0;
	//Code for printing only users
	if (exclusion_code == 2)
	{
		for(int i = 0; i < samples; i++)
        {
			system("clear");
            print_header(samples, tick_time);
			print_user_section();
			cpu_use = print_system_ending(cpu_use, &idle_proc);
            sleep(tick_time);
        }
		print_system_info();
		return;
	}
	char old_str[samples][255];
	char str[255];
    struct sysinfo pointer;
	float total_physical_mem = 0;
	float used_phy_mem = 0;
	float total_virt_mem = 0;
	float used_virt_mem = 0;
	//Print only system
	if (exclusion_code == 1)
	{
		for(int i = 0; i < samples; i++)
        {
			system("clear");
			print_header(samples, tick_time);
			print_system_use();
			for(int j = 0; j < i; j++)
            {
                printf("%s", old_str[j]);
            }
			sysinfo(&pointer);
    		total_physical_mem = ((float)(pointer.totalram)) / 1000000000;
    		used_phy_mem = total_physical_mem - (((float)(pointer.freeram)) / 1000000000);
    		total_virt_mem = total_physical_mem + (((float)(pointer.totalswap)) / 1000000000);
    		used_virt_mem = total_virt_mem - ((float)(pointer.freeswap) / 1000000000) - (((float)(pointer.freeram)) / 1000000000);
    		sprintf(str, "%.4f / %.4f GB -- %.4f / %.4f GB\n", used_phy_mem, total_physical_mem, used_virt_mem, total_virt_mem);
			strncpy(old_str[i], str, 254);
            int rest_of_the_lines = samples - i;
            for(int j = 1; j < rest_of_the_lines; j++)
            {
                printf("\n");
            }
			printf("____________________________\n");
            cpu_use = print_system_ending(cpu_use, &idle_proc);
			sleep(tick_time);
        }
		print_system_info();
		return;
	}
	//Must print all values
	if (exclusion_code == 0)
	{
		for(int i = 0; i < samples; i++)
        {
			system("clear");
			print_header(samples, tick_time);
			print_system_use();
			sysinfo(&pointer);
    		total_physical_mem = ((float)(pointer.totalram)) / 1000000000;
    		used_phy_mem = total_physical_mem - (((float)(pointer.freeram)) / 1000000000);
    		total_virt_mem = total_physical_mem + (((float)(pointer.totalswap)) / 1000000000);
    		used_virt_mem = total_virt_mem - ((float)(pointer.freeswap) / 1000000000) - (((float)(pointer.freeram)) / 1000000000);
    		sprintf(str, "%.4f / %.4f GB -- %.4f / %.4f GB\n", used_phy_mem, total_physical_mem, used_virt_mem, total_virt_mem);
			strncpy(old_str[i], str, 254);
			for(int j = 0; j < i+1; j++)
            {
                printf("%s",old_str[j]);
            }
            int rest_of_the_lines = samples - i;
            for(int j = 1; j < rest_of_the_lines; j++)
            {
                printf("\n");
            }
			printf("____________________________\n");
			print_user_section();
            cpu_use = print_system_ending(cpu_use, &idle_proc);
			sleep(tick_time);
        }
		print_system_info();
		return;
	}
}
//Helper function to write the graphics arguments in fancy_sequential. Due to the the need to save values in print_fancy_normal after the screen has been cleared, it is hardcoded directly in print_fancy_normal
long print_fancy_system_ending_seq(long old_cpu_use, long *old_idle)
{
	//Initialize our variables to read from the file
	char new_line[4];
	int core_amt = 0;
	char cpu_total[255];
	//Open the file for reading
	FILE *file = fopen("/proc/stat", "r");
	//Get the first line, which holds the cpu use data
	fgets(cpu_total, 255, file);
	long new_idle;
	long cpu_use = get_cpu_use(cpu_total, &new_idle);
	float print_val = 0;
	if (old_cpu_use != 0)
	{
		int cp_diff = (cpu_use - old_cpu_use);
		int idle_diff = (new_idle - *old_idle);
		int actual_cp = cp_diff - idle_diff;
		print_val = (float)(actual_cp)/((cpu_use + old_cpu_use)-(new_idle + *old_idle));
	}
	char *end_check = fgets(new_line, 4, file);
	while(end_check)
	{
		//Get the first 3 characters for each line after the first. For each cpu, the new line will begin with the text "cpu" so we can list how many cpus there are that way
		if (strcmp(new_line, "cpu") == 0)
		{
			core_amt++;
		}
		end_check = fgets(new_line, 4, file);
	}
	print_val = print_val * 100;
	*old_idle = new_idle; 
	fclose(file);
    printf("Number of cores: %d\n", core_amt);
	printf("\tCPU Usage: %.4f %%\n", print_val);
	printf("\t\t ||");
	for(float f = 0; f < print_val; f += .005)
	{
		printf("|");
	}
	printf("%.2f\n", print_val);
    printf("____________________________\n");
	return(cpu_use);
}

//Same as above method, except now we must account for saving our old strings to print them out after each iteration
long print_fancy_system_ending_normal(long old_cpu_use, long *old_idle, float *old_cpu_uses[], int index)
{
		//Initialize our variables to read from the file
	char new_line[4];
	int core_amt = 0;
	char cpu_total[255];
	//Open the file for reading
	FILE *file = fopen("/proc/stat", "r");
	//Get the first line, which holds the cpu use data
	fgets(cpu_total, 255, file);
	long new_idle;
	long cpu_use = get_cpu_use(cpu_total, &new_idle);
	float print_val = 0;
	if (old_cpu_use != 0)
	{
		int cp_diff = (cpu_use - old_cpu_use);
		int idle_diff = (new_idle - *old_idle);
		int actual_cp = cp_diff - idle_diff;
		print_val = (float)(actual_cp)/((cpu_use + old_cpu_use)-(new_idle + *old_idle));
	}
	char *end_check = fgets(new_line, 4, file);
	while(end_check)
	{
		//Get the first 3 characters for each line after the first. For each cpu, the new line will begin with the text "cpu" so we can list how many cpus there are that way
		if (strcmp(new_line, "cpu") == 0)
		{
			core_amt++;
		}
		end_check = fgets(new_line, 4, file);
	}
	print_val = print_val * 100;
	*old_idle = new_idle; 
	fclose(file);
    printf("Number of cores: %d\n", core_amt);
	printf("\tCPU Usage: %.4f %%\n", print_val);
	*(old_cpu_uses[index]) = print_val;
	for (int i = 0; i < index; i++)
	{
		printf("\t\t ||");
		for(float f = 0; f < print_val; f += .05)
		{
			printf("|");
		}
		printf("%.2f\n", *(old_cpu_uses[i]));
	}
    printf("____________________________\n");
	return(cpu_use);
}

//Prints the system samples in a sequential format when --graphics is passed
float print_fancy_system_samples(float old_used_phy_mem)
{
    struct sysinfo pointer;
	sysinfo(&pointer);
    float total_physical_mem = ((float)(pointer.totalram)) / 1000000000;
    float used_phy_mem = total_physical_mem - (((float)(pointer.freeram)) / 1000000000);
    float total_virt_mem = total_physical_mem + (((float)(pointer.totalswap)) / 1000000000);
    float used_virt_mem = total_virt_mem - ((float)(pointer.freeswap) / 1000000000) - (((float)(pointer.freeram)) / 1000000000);
    printf("%.4f / %.4f GB -- %.4f / %.4f GB", used_phy_mem, total_physical_mem, used_virt_mem, total_virt_mem);
	printf("\t|");
	if (old_used_phy_mem == 0)
	{
		printf("o 0.00 (%.4f)\n", used_phy_mem);
	}
	else
	{
		float diff = fabs(used_phy_mem-old_used_phy_mem);
		for(float i = .01; i < diff; i += .01)
		{
			printf("#");
		}
			printf("* %.4f (%.4f)\n", diff, used_phy_mem);
	}
	return (used_phy_mem);
}

//Prints sequentially and graphically (--sequential and --graphics are both passed as arguments)
void print_fancy_seq(int samples, int tick_time, int ex_code)
{
	int process_mem = print_header(samples, tick_time);
	long cpu_use = 0;
	long idle_proc = 0;
	float old_phys_mem = 0;
	//Code for printing everything
    if(ex_code == 0)
    {
		for(int i = 0; i < samples; i++)
        {
            printf(">>>Iteration %d\n", i);
            printf("Memory usage: %d kilobytes\n", process_mem);
			printf("____________________________\n");
            print_system_use();
            for(int j = 0; j < i; j++)
            {
                printf("\n");
            }
            old_phys_mem = print_fancy_system_samples(old_phys_mem);
            int rest_of_the_lines = samples - i;
            for(int j = 1; j < rest_of_the_lines; j++)
            {
                printf("\n");
            }
			printf("____________________________\n");
			print_user_section();
            cpu_use = print_fancy_system_ending_seq(cpu_use, &idle_proc);
			sleep(tick_time);
        }
    }
	//Code for only printing system
	if (ex_code == 1)
	{
        for(int i = 0; i < samples; i++)
        {
            printf(">>>Iteration %d\n", i);
            printf("Memory usage: %d kilobytes\n", process_mem);
			printf("____________________________\n");
            print_system_use();
            for(int j = 0; j < i; j++)
            {
                printf("\n");
            }
            old_phys_mem = print_fancy_system_samples(old_phys_mem);
            int rest_of_the_lines = samples - i;
            for(int j = 1; j < rest_of_the_lines; j++)
            {
                printf("\n");
            }
			printf("____________________________\n");
            cpu_use = print_fancy_system_ending_seq(cpu_use, &idle_proc);
			sleep(tick_time);
        }
    }
    //Code for only printing users
    if (ex_code == 2)
    {
        for(int i = 0; i < samples; i++)
        {
			printf(">>>Iteration %d\n", i);
            printf("Memory usage: %d kilobytes\n", process_mem);
			printf("____________________________\n");
			print_user_section();
			cpu_use = print_fancy_system_ending_seq(cpu_use, &idle_proc);
            sleep(tick_time);
        }
    }
	print_system_info();
}

//graphics method must be implemented differently in order to account for the extra printing we need
void print_fancy_normal(int samples, int tick_time, int ex_code)
{
	long cpu_use = 0;
	long idle_proc = 0;
	float *old_cpu_uses[samples];
	int index = 0;
	//Code for printing only users
	if (ex_code == 2)
	{
		for(int i = 0; i < samples; i++)
        {
			system("clear");
            print_header(samples, tick_time);
			print_user_section();
			cpu_use = print_fancy_system_ending_normal(cpu_use, &idle_proc, old_cpu_uses, index);
			index++;
            sleep(tick_time);
        }
		print_system_info();
		return;
	}
	char old_str[samples][255];
	char str[255];
    struct sysinfo pointer;
	float total_physical_mem = 0;
	float used_phy_mem = 0;
	float old_used_phy_mem = 0;
	float total_virt_mem = 0;
	float used_virt_mem = 0;
	//Print only system
	if (ex_code == 1)
	{
		for(int i = 0; i < samples; i++)
        {
			system("clear");
			print_header(samples, tick_time);
			print_system_use();
			for(int j = 0; j < i; j++)
            {
                printf("%s", old_str[j]);
            }
			sysinfo(&pointer);
    		total_physical_mem = ((float)(pointer.totalram)) / 1000000000;
    		used_phy_mem = total_physical_mem - (((float)(pointer.freeram)) / 1000000000);
    		total_virt_mem = total_physical_mem + (((float)(pointer.totalswap)) / 1000000000);
    		used_virt_mem = total_virt_mem - ((float)(pointer.freeswap) / 1000000000) - (((float)(pointer.freeram)) / 1000000000);
    		sprintf(str, "%.4f / %.4f GB -- %.4f / %.4f GB", used_phy_mem, total_physical_mem, used_virt_mem, total_virt_mem);
			if (i > 0)
			{
				printf("\t|");
				if (old_used_phy_mem == 0)
				{
					printf("o 0.00 (%.4f)\n", used_phy_mem);
				}
				else
				{
					float diff = used_phy_mem-old_used_phy_mem;
					for(float i = 0; i < (used_phy_mem-old_used_phy_mem); i += .01)
					{
						printf("#");
					}
					printf("* %.4f (%.4f)\n", diff, used_phy_mem);
				}
			}			
			old_used_phy_mem = used_phy_mem;
			strncpy(old_str[i], str, 254);
            int rest_of_the_lines = samples - i;
            for(int j = 1; j < rest_of_the_lines; j++)
            {
                printf("\n");
            }
			printf("____________________________\n");
			cpu_use = print_fancy_system_ending_normal(cpu_use, &idle_proc, old_cpu_uses, index);
			index++;
			sleep(tick_time);
        }
		print_system_info();
		return;
	}
	//Must print all values
	if (ex_code == 0)
	{
		for(int i = 0; i < samples; i++)
        {
			system("clear");
			print_header(samples, tick_time);
			print_system_use();
			for(int j = 0; j < i; j++)
            {
                printf("%s", old_str[j]);
            }
			sysinfo(&pointer);
    		total_physical_mem = ((float)(pointer.totalram)) / 1000000000;
    		used_phy_mem = total_physical_mem - (((float)(pointer.freeram)) / 1000000000);
    		total_virt_mem = total_physical_mem + (((float)(pointer.totalswap)) / 1000000000);
    		used_virt_mem = total_virt_mem - ((float)(pointer.freeswap) / 1000000000) - (((float)(pointer.freeram)) / 1000000000);
    		sprintf(str, "%.4f / %.4f GB -- %.4f / %.4f GB", used_phy_mem, total_physical_mem, used_virt_mem, total_virt_mem);
			printf("\t|");
			if (old_used_phy_mem == 0)
			{
				printf("o 0.00 (%.4f)\n", used_phy_mem);
			}
			else
			{
				float diff = used_phy_mem-old_used_phy_mem;
				for(float i = 0; i < (used_phy_mem-old_used_phy_mem); i += .01)
				{
					printf("#");
				}
				printf("* %.4f (%.4f)\n", diff, used_phy_mem);
			}
			old_used_phy_mem = used_phy_mem;
			strncpy(old_str[i], str, 254);
            int rest_of_the_lines = samples - i;
            for(int j = 1; j < rest_of_the_lines; j++)
            {
                printf("\n");
            }
			printf("____________________________\n");
			print_user_section();
			cpu_use = print_fancy_system_ending_normal(cpu_use, &idle_proc, old_cpu_uses, index);
			index++;
			sleep(tick_time);
        }
		print_system_info();
		return;
	}
}
//Helper function to parse all the arguments from the user and set the value at pointers accordingly. In case of invalid input, a 1 will be returned, if run smoothly, a 0 will be returned
int take_awguments_UwU(int argc, char **argv, int *samples, int *tick_time, bool *system_only, bool *fancy, bool *user_only, bool *sequential)
{
	bool samples_init = false;
	bool tick_time_init = false;
	for(int i = 1; i < argc; i++)
	{
		if(strcmp(*(argv+i), "--system") == 0)
		{
			*(system_only) = true;
		}
		if(strcmp(*(argv+i), "--user") == 0)
		{
			*(user_only) = true;
		}
		if(strcmp(*(argv+i), "--graphics") == 0 || strcmp(*(argv+i), "--g") == 0)
		{
			*(fancy) = true;
		}
		if(strcmp(*(argv+i), "--sequential") == 0)
		{
			*(sequential) = true;
		}
		if (strncmp(*(argv+i), "--tdelay=", 9) == 0 && !(tick_time_init))
		{
			if(isdigit((*(argv+i))[9]) > 0)
			{
				char *checker = memchr(*(argv+i), '=', 20);
				char num_str[10];
				for(int j = 1; *(checker+j) && j < 11; j++)
				{
					num_str[i-1] = *(checker+i);
				}
				printf("%s", num_str);
				*(tick_time) = strtol(num_str, NULL, 10);
				tick_time_init = true;
			}
			else
			{
				printf("Invalid input (--tdelay)");
				return 1;
			}
		}
		if (isdigit(**(argv+i)) && samples_init && !(tick_time_init))
		{
			*(tick_time) = strtol((*(argv+i)), NULL, 10);
			tick_time_init = true;
		}
		if (strncmp(*(argv+i), "--samples=", 10) == 0 && !(samples_init))
		{
			if(isdigit((*(argv+i))[10]) > 0)
			{
				char *checker = memchr(*(argv+i), '=', 20);
				char num_str[10];
				for(int i = 1; *(checker+i) && i < 11; i++)
				{
					num_str[i-1] = *(checker+i);
				}
				printf("%s", num_str);
				*(samples) = strtol(num_str, NULL, 10);
				samples_init = true;
			}
			else
			{
				printf("Invalid input (--samples)");
				return 1;
			}
		}
		if (isdigit(**(argv+i)) && !(samples_init))
		{
			*(samples) = strtol(*(argv+i), NULL, 10);
			samples_init = true;
		}
	}
	return 0;
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
    //Cover the case of user being an idiot and not reading documentation and inputting exclusive arguments or incorrect arguments after tdelay or samples
	int err_code = take_awguments_UwU(argc, argv, &samples, &tick_time, &system_only, &fancy, &user_only, &sequential);
	if (err_code == 1)
	{
		fprintf(stderr, "Error: The values following samples= and tdelay= must be numeric");
		return 1;
	}
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
		if (fancy)
		{
			print_fancy_normal(samples, tick_time, exclusion_code);
			return 0;
		}
		print_normal(samples, tick_time, exclusion_code);
		return 0;
	}
	//At this point, we know that we must print all metrics
	if (sequential && ! fancy)
	{
		print_sequential(samples, tick_time, 0);
		return 0;
	}
	//Check if user wants the graphics argument
	if (fancy)
	{
		if (sequential)
		{
			print_fancy_seq(samples, tick_time, 0);
			return 0;
		}
		print_fancy_normal(samples, tick_time, 0);
		return 0;
	}
	print_normal(samples, tick_time, 0);
    return 0;
}
//TODO: Implement graphics argument
