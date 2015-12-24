#include <stdlib.h>
#include <stdio.h>
#include "parsing_command.h"
#include "job_struct.h"

int main(int argc, char *argv[])
{
    int error = 0, num_job_todo = -1;
    char c = '\n';
    struct job c_job;
    initializationOfGlobalVariables(argc, argv);
    
    signal(SIGINT,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);
    signal(SIGCHLD, child_proc_ended);
    //printf("%i\n", getpid());
    
    while (1)
    {
        c_job.mas_programs = NULL;
        c_job.number_programs = 0;
        c_job.status = -1;
        c_job.pid_helper = -1;
        
        if (c == '\n')
        {
            while (num_job_todo != -1)
            {
                num_job_todo = find_job();
                error = job_man(num_job_todo);
                if (error == -3)
                    return 0;
            }
            
            printf("\x1b[1;31m%s$ \x1b[0m", globalVariables.username);
        }
        
        c = read_command(&c_job);
        
        if (c != -2)
        {
            num_job_todo = -2;
            add_job(c_job);
        }
        
        if (c == -2)
            c = '\n';
        
    }
    return 0;
}
