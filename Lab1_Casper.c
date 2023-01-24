#include <stdio.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <string.h>
//This is a structure of all the possible information we will need to present to the user
struct info_of_comp
{
    int mem_usage;
    float physical_mem_used;
    float physical_mem_total;
    float virtual_mem_used;
    float virtual_mem_total;
    char *user;

}


//Prints the header, will be called regardless of the arguments afterwards
void print_header(int mem_usage, int sample_amt, int tick_time)
{
    printf("Number of Samples: %d -- every %lf seconds\n", sample_amt, tick_time);
    printf("Memory usage: %d\n", mem_usage);
    printf("_____________________________________________________\n");
}




//Prints system use in the case that --system is called or no limiting arguments are called
void print_system_use(int sample_amt, int tick_time)
{
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
    //for loop for outputting the memory recieved by each function
    for (int i = 0; i < sample_amt; i++)
    {
        print_system_samples();
    }
    printf("_____________________________________________________\n");
}



//Gets hardware data and prints the system use line
void print_system_samples()
{
    info_of_comp *data_pointer = get_data();
    printf("%lf / %lf GB -- %lf / %lf GB\n", physical_mem_used, physical_mem_total, virtual_mem_used, virtual_mem_total);
}

void print_user_section(int tick_time, int sample_amt)
{
    printf("### Sessions/users ###\n");
    for (int i = 0; i < sample_amt; i++)
    {
        print_user_line();
        getUserInfo()
        printf("%s          %s  (%s)", *username, *dir, *operation);
    }

}
void print

//Gets the data using imported check libraries
info_of_comp *get_data()
{
    info_of_comp *pointer = calloc(sizeof(info_of_comp), 1);
    //WHHHHHHHHHhATATATATATATAt
    return (pointer);
}


int main(int argc, char **argv)
{
    if (argc == 1)
    {
        getSystemInfo
        //Something here (No arguments being passed)
        return 0;
    }
    

}
