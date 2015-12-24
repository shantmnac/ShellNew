#include "job_struct.h"

struct job *mas_of_jobs = NULL;
int number_jobs = 0;

struct job *mas_of_jobs_in_bg = NULL;
int number_jobs_in_bg = 0;

int last_job_fg = -1;

int now_job;

int flag = 0;

int flag2 =0;


void print_job(struct job *job)
{
    int i, j;
    for (i = 0; i < job->number_programs; i ++)
    {
        printf("%s", job->mas_programs[i].name);
        for (j = 1; j < job->mas_programs[i].number_of_arguments; j++)
        {
            printf(" %s", job->mas_programs[i].arguments[j]);
        }
        if (job->number_programs > 1)
            printf("| ");
    }
    printf("\n");
}

void print_jobs()
{
    int i = 1, j;
    printf("\n*************************************");
    printf("Stopped:\n");
    for (j = 0; j < number_jobs; j++)
    {
        if (mas_of_jobs[j].status == 3)
        {
            printf("[%i]: GPID %i, ", i, mas_of_jobs[j].pid_helper);
            print_job(&mas_of_jobs[j]);
            i++;
        }
    }
    
    printf("\n*************************************");
    printf("In background:\n");
    i = 1;
    for (j = 0; j < number_jobs_in_bg; j++)
    {
        if (mas_of_jobs_in_bg[j].status == 1)
        {
            printf("[%i]: GPID %i, ", i, mas_of_jobs_in_bg[j].pid_helper);
            print_job(&mas_of_jobs_in_bg[j]);
            i++;
        }
    }
    printf("\n*************************************\n");
}

int free_globals()
{
    free(globalVariables.currentDirectory);
    free(globalVariables.shellDirectory);
    free(globalVariables.argv);
    
    return 0;
}

int initializationOfGlobalVariables(int argc, char *argv[])
{
    int i;
    struct passwd *pw = getpwuid(getuid());
    
    globalVariables.argv = (char**)malloc(argc * sizeof(char*));
    
    for (i = 0; i < argc; i++) {
        globalVariables.argv[i] = argv[i];
    }
    
    globalVariables.homeDirectory = pw->pw_dir;
    globalVariables.PID = (int)getpid();
    globalVariables.UID = (int)getuid();
    globalVariables.argc = argc;
    globalVariables.statusOfLastProcess = 0;
    globalVariables.shellDirectory = getcwd(globalVariables.shellDirectory, 0);
    globalVariables.currentDirectory = getcwd(globalVariables.currentDirectory, 0);
    globalVariables.username = getlogin();
    return 0;
}

int cd(char *c)
{
    chdir(c);
    globalVariables.currentDirectory = getcwd(globalVariables.currentDirectory, 0);
    return 0;
}

void ctrl_c_pushed(int sig)
{
    int i;
    for (i = 0; i < mas_of_jobs[now_job].number_programs; i++)
    {
        kill(mas_of_jobs[now_job].mas_programs[i].pid, SIGKILL);
    }

    signal(SIGINT, ctrl_c_pushed);
}

void ctrl_z_pushed(int sig)
{
    int i;
    for (i = 0; i < mas_of_jobs[now_job].number_programs; i++)
    {
        kill(mas_of_jobs[now_job].mas_programs[i].pid, SIGSTOP);
    }
    
    flag = 1;
    tcsetpgrp(0, getppid());
    raise(SIGSTOP);
    signal(SIGTSTP, ctrl_z_pushed);
}

int delete_job(int number_of_job)
{
    struct job *c_job;
    struct program *c_program;
    int i, j;
    c_job = &mas_of_jobs[number_of_job];
    for (i = 0; i < c_job->number_programs; i++)
    {
        c_program = &c_job->mas_programs[i];
        free(c_program->output_file);
        free(c_program->name);
        free(c_program->input_file);
        for (j = 0; j < c_program->number_of_arguments; j++)
        {
            free(c_program->arguments[j]);
        }
        free(c_program->arguments);
    }
    free(c_job->mas_programs);
    for (i = number_of_job; i < number_jobs - 1; i++)
    {
        mas_of_jobs[i] = mas_of_jobs[i+1];
    }
    
    number_jobs--;
    
    if (number_jobs != 0)
        mas_of_jobs = realloc(mas_of_jobs, sizeof(struct job) * number_jobs);
    else
    {
        free(mas_of_jobs);
        mas_of_jobs = NULL;
    }
    
    return 0;
}

int delete_job_in_mas_of_bg(int number_of_job)
{
    struct job *c_job;
    struct program *c_program;
    int i, j;
    c_job = &mas_of_jobs_in_bg[number_of_job];
    for (i = 0; i < c_job->number_programs; i++)
    {
        c_program = &c_job->mas_programs[i];
        free(c_program->output_file);
        free(c_program->name);
        free(c_program->input_file);
        for (j = 0; j < c_program->number_of_arguments; j++)
        {
            free(c_program->arguments[j]);
        }
        free(c_program->arguments);
    }
    free(c_job->mas_programs);
    for (i = number_of_job; i < number_jobs_in_bg - 1; i++)
    {
        mas_of_jobs_in_bg[i] = mas_of_jobs_in_bg[i+1];
    }
    
    number_jobs_in_bg--;
    
    if (number_jobs_in_bg != 0)
        mas_of_jobs_in_bg = realloc(mas_of_jobs_in_bg, sizeof(struct job) * number_jobs_in_bg);
    else
    {
        free(mas_of_jobs_in_bg);
        mas_of_jobs_in_bg = NULL;
    }
    
    return 0;
}

int free_mas_of_jobs()
{
    struct job *c_job;
    struct program *c_program;
    int i, j, k;
    
    if (number_jobs == 0)
        return 0;
    
    for( i = 0; i < number_jobs; i++)
    {
        c_job = &mas_of_jobs[i];
        
        for (j = 0; j < c_job->number_programs; j++)
        {
            c_program = &c_job->mas_programs[j];
            
            if (c_program->output_file != NULL)
                free(c_program->output_file);
            if (c_program->name != NULL)
                free(c_program->name);
            if (c_program->input_file != NULL)
                free(c_program->input_file);
            for (k = 0; k < c_program->number_of_arguments; k++)
            {
                free(c_program->arguments[k]);
            }
            if (c_program->arguments != NULL)
                free(c_program->arguments);
        }
        if (c_job->mas_programs != NULL)
            free(c_job->mas_programs);
    }
    
    free(mas_of_jobs);
    number_jobs = 0;
    
    return 0;
}

int free_mas_of_jobs_in_bg()
{
    struct job *c_job;
    struct program *c_program;
    int i, j, k;
    
    if (number_jobs_in_bg == 0)
        return 0;
    
    for( i = 0; i < number_jobs_in_bg; i++)
    {
        c_job = &mas_of_jobs_in_bg[i];
        
        for (j = 0; j < c_job->number_programs; j++)
        {
            c_program = &c_job->mas_programs[j];
            
            if (c_program->output_file != NULL)
                free(c_program->output_file);
            if (c_program->name != NULL)
                free(c_program->name);
            if (c_program->input_file != NULL)
                free(c_program->input_file);
            for (k = 0; k < c_program->number_of_arguments; k++)
            {
                free(c_program->arguments[k]);
            }
            if (c_program->arguments != NULL)
                free(c_program->arguments);
        }
        if (c_job->mas_programs != NULL)
            free(c_job->mas_programs);
    }
    
    free(mas_of_jobs_in_bg);
    number_jobs_in_bg = 0;
    
    return 0;
}

int job_man_bg(struct job *job)
{
    int fd1[2], fd2[2], i;
    pid_t c_pid, pid;
    struct program *c_program;
    
    c_pid = fork();
    
    if (c_pid != 0)
    {
        job->pid_helper = c_pid;
        return 0;
    }
    
    setpgid(getpid(), getpid());
    
    signal(SIGCHLD, SIG_IGN);
    signal(SIGTTIN,SIG_DFL);
    signal(SIGINT,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);

    if (job->number_programs == 1)
    {
        c_program = &job->mas_programs[0];
        c_program->arguments = realloc(c_program->arguments, sizeof(char *) * (c_program->number_of_arguments + 1));
        c_program->arguments[c_program->number_of_arguments] = (char *) 0;
        c_program->number_of_arguments++;
        
        if (!strcmp(c_program->name, "exit"))
        {
            free_mas_of_jobs();
            free_globals();
            return -3;
        }
        
        c_pid = fork();
        if (c_pid != 0)
            c_program->pid = c_pid;
        
        if (!c_pid)
        {
            signal(SIGTTIN,SIG_DFL);
            setpgid(c_program->pid, getppid());
            if (c_program->input_file!=NULL)
            {
                if ((fd1[0]=open(c_program->input_file,O_RDONLY))==-1)
                {
                    perror("open");
                    return -1;
                }
                dup2(fd1[0],0);
                close(fd1[0]);
            }
            
            if (c_program->output_file!=NULL)
            {
                if (c_program->output_type==1)
                {
                    if ((fd1[1]=open(c_program->output_file,O_CREAT|O_WRONLY,0666))==-1)
                    {
                        perror("open");
                        return -1;
                    }
                }
                else
                    if ((fd1[1]=open(c_program->output_file,O_CREAT|O_WRONLY|O_APPEND,0666))==-1)
                    {
                        perror("open");
                        return -1;
                    }
                dup2(fd1[1],1);
                close(fd1[1]);
            }
            
            if (!strcmp(c_program->name, "jobs"))
            {
                now_job = -1;
                free_mas_of_jobs();
                free_globals();
                exit(0);
            }
            
            if (!strcmp(c_program->name, "mcat"))
            {
                if (c_program->number_of_arguments == 2)
                    mcat(NULL);
                
                if (c_program->number_of_arguments == 3)
                    mcat(c_program->arguments[1]);
                
                now_job = -1;
                free_mas_of_jobs();
                free_globals();
                exit(0);
            }
            
            if (!strcmp(c_program->name, "msed"))
            {
                if (c_program->number_of_arguments != 4)
                {
                    now_job = -1;
                    free_mas_of_jobs();
                    free_globals();
                    exit(0);
                }
                
                msed(c_program->arguments[1], c_program->arguments[2]);
                
                now_job = -1;
                free_mas_of_jobs();
                free_globals();
                exit(0);
            }
            
            if (!strcmp(c_program->name, "mgrep"))
            {
                if ((c_program->number_of_arguments > 4) && (c_program->number_of_arguments <= 2))
                {
                    now_job = -1;
                    free_mas_of_jobs();
                    free_globals();
                    exit(0);
                }
                if (c_program->number_of_arguments == 3)
                {
                    mgrep(c_program->arguments[1], 0);
                    now_job = -1;
                    free_mas_of_jobs();
                    free_globals();
                    exit(0);
                }
                if (c_program->number_of_arguments == 4)
                {
                    if (!strcmp(c_program->arguments[3], "-v"))
                    {
                        mgrep(c_program->arguments[1], 1);
                        now_job = -1;
                        return 0;
                    }
                    now_job = -1;
                    free_mas_of_jobs();
                    free_globals();
                    exit(0);
                }
            }

            execvp(c_program->name, c_program->arguments);
            perror("exec");
            free_mas_of_jobs();
            free_globals();
            exit(-1);
        }
    }
    else
    {
        pipe(fd1);
        for (i = 0; i < job->number_programs; i++)
        {
            c_program = &job->mas_programs[i];
            c_program->arguments = realloc(c_program->arguments, sizeof(char *) * (c_program->number_of_arguments + 1));
            c_program->arguments[c_program->number_of_arguments] = (char *) 0;
            c_program->number_of_arguments++;
            
            pipe(fd2);
            c_pid = fork();
            
            c_program->pid = c_pid;
            
            if (!c_pid)
            {
                signal(SIGTTIN,SIG_DFL);
                setpgid(c_program->pid, getppid());
                if (!i)
                {
                    dup2(fd2[1],1);
                }
                else if (i < job->number_programs - 1)
                {
                    dup2(fd1[0],0);
                    dup2(fd2[1],1);
                }
                else
                {
                    dup2(fd1[0],0);
                }
                
                close(fd1[0]);
                close(fd1[1]);
                close(fd2[0]);
                close(fd2[1]);
                
                if (c_program->input_file!=NULL)
                {
                    if ((fd1[0]=open(c_program->input_file,O_RDONLY)) == -1)
                    {
                        perror("open");
                        return -1;
                    }
                    dup2(fd1[0],0);
                    close(fd1[0]);
                }
                
                if (c_program->output_file!=NULL)
                {
                    if (c_program->output_type==1)
                    {
                        if ((fd1[1]=open(c_program->output_file,O_CREAT|O_WRONLY,0666))==-1)
                        {
                            perror("open");
                            return -1;
                        }
                    }
                    else
                        if ((fd1[1]=open(c_program->output_file,O_CREAT|O_WRONLY|O_APPEND,0666))==-1)
                        {
                            perror("open");
                            return -1;
                        }
                    dup2(fd1[1],1);
                    close(fd2[1]);
                }
                
                if (!strcmp(c_program->name, "jobs"))
                {
                    now_job = -1;
                    free_mas_of_jobs();
                    free_globals();
                    exit(0);
                }
                
                if (!strcmp(c_program->name, "mcat"))
                {
                    if (c_program->number_of_arguments == 2)
                        mcat(NULL);
                    
                    if (c_program->number_of_arguments == 3)
                        mcat(c_program->arguments[1]);
                    
                    now_job = -1;
                    free_mas_of_jobs();
                    free_globals();
                    exit(0);
                }
                
                if (!strcmp(c_program->name, "msed"))
                {
                    if (c_program->number_of_arguments != 4)
                    {
                        now_job = -1;
                        free_mas_of_jobs();
                        free_globals();
                        exit(0);
                    }
                    
                    msed(c_program->arguments[1], c_program->arguments[2]);
                    
                    now_job = -1;
                    free_mas_of_jobs();
                    free_globals();
                    exit(0);
                }
                
                if (!strcmp(c_program->name, "mgrep"))
                {
                    if ((c_program->number_of_arguments > 4) && (c_program->number_of_arguments <= 2))
                    {
                        now_job = -1;
                        free_mas_of_jobs();
                        free_globals();
                        exit(0);
                    }
                    if (c_program->number_of_arguments == 3)
                    {
                        mgrep(c_program->arguments[1], 0);
                        now_job = -1;
                        free_mas_of_jobs();
                        free_globals();
                        exit(0);
                    }
                    if (c_program->number_of_arguments == 4)
                    {
                        if (!strcmp(c_program->arguments[3], "-v"))
                        {
                            mgrep(c_program->arguments[1], 1);
                            now_job = -1;
                            free_mas_of_jobs();
                            free_globals();
                            exit(0);
                        }
                        now_job = -1;
                        free_mas_of_jobs();
                        free_globals();
                        exit(0);
                    }
                }
                
                execvp(c_program->name, c_program->arguments);
                perror("exec");
                free_mas_of_jobs();
                free_globals();
            }
            
            close(fd1[0]);
            close(fd1[1]);
            fd1[0]=fd2[0];
            fd1[1]=fd2[1];
        }
        close(fd1[0]);
        close(fd1[1]);
    }
    
    errno  = 0;
    while ((pid = waitpid(-1, NULL, WUNTRACED)))
    {
        if (errno == ECHILD)
            break;
    }
    
    free_mas_of_jobs();
    free_globals();
    exit(0);
}

int fg(struct job *fg_job)
{
    int i, j = 0;
    
    struct job *c_job;

    if (fg_job->number_programs > 1)
        return -1;
    if ((fg_job->mas_programs[0].number_of_arguments != 3))
        return -1;
    
    if (!strcmp(fg_job->mas_programs[0].arguments[1], "s"))
    {
        
        for (i = 0; i < number_jobs; i++)
        {
            if ((j == atoi(fg_job->mas_programs[0].arguments[2])-1) && (mas_of_jobs[j].status == 3))
                break;
            
            if (mas_of_jobs[i].status == 3)
                j++;
        }

        c_job = &mas_of_jobs[i];
        
        mas_of_jobs[i].status = 2;
        printf("%i", c_job->pid_helper);
        
        tcsetpgrp(0, c_job->pid_helper);
        
        for (i = 0;  i < c_job->number_programs; i++)
        {
            kill(c_job->mas_programs[i].pid, SIGCONT);
        }
        
        kill(c_job->pid_helper, SIGCONT);
        sleep(1);
        waitpid(c_job->pid_helper, NULL, 0);
        
        now_job = -1;
        
        return 0;
    }
    
    if (!strcmp(fg_job->mas_programs[0].arguments[1], "b"))
    {
        for (i = 0; i < number_jobs_in_bg; i++)
        {
            if ((j == atoi(fg_job->mas_programs[0].arguments[2])-1) && (mas_of_jobs_in_bg[j].status == 1))
                break;
            
            if (mas_of_jobs_in_bg[i].status == 1)
                j++;
        }
        
        c_job = &mas_of_jobs_in_bg[i];
        
        mas_of_jobs[i].status = 2;
        
        add_job(*c_job);
        
        delete_job_in_mas_of_bg(i);
        
        tcsetpgrp(0, c_job->pid_helper);
        
        for (i = 0;  i < c_job->number_programs; i++)
        {
            kill(c_job->mas_programs[i].pid, SIGCONT);
        }
        
        kill(c_job->pid_helper, SIGCONT);
        sleep(1);
        waitpid(c_job->pid_helper, NULL, 0);
        
        now_job = -1;

        return 0;
    }
    
    
    
    return -1;
}

int job_man(int num_job)
{
    int fd1[2], fd2[2], i;
    pid_t c_pid, c_pid2, pid;
    struct program *c_program;
    
    if (num_job == -1)
        return 0;
    
    if (number_jobs == 0)
        return 0;
    
    if (!strcmp(mas_of_jobs[num_job].mas_programs[0].name, "exit"))
    {
        free_mas_of_jobs();
        free_globals();
        return -3;
    }
    
    if (!strcmp(mas_of_jobs[num_job].mas_programs[0].name, "fg"))
    {
        flag = 0;
        fg(&mas_of_jobs[num_job]);
        delete_job(num_job-flag);
        flag = 0;
        now_job = -1;
        return 0;
    }
    
    now_job = num_job;
    
    c_pid2 = fork();
    if (c_pid2 != 0)
    {
        while (getpgid(getpid()) == getpgid(c_pid2)){;}
        
        tcsetpgrp(0, c_pid2);
        
        mas_of_jobs[now_job].pid_helper = c_pid2;
        
        waitpid(c_pid2, NULL, 0);
        
        now_job = -1;
        
        return 0;;
    }
    
    signal(SIGCHLD, SIG_IGN);
    signal(SIGTTIN,SIG_DFL);
    signal(SIGINT,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);
    
    setpgid(getpid(), getpid());
    
    if (mas_of_jobs[num_job].number_programs == 1)
    {
        c_program = &mas_of_jobs[num_job].mas_programs[0];
        c_program->arguments = realloc(c_program->arguments, sizeof(char *) * (c_program->number_of_arguments + 1));
        c_program->arguments[c_program->number_of_arguments] = (char *) 0;
        c_program->number_of_arguments++;
        
        if (!strcmp(c_program->name, "cd"))
        {
            cd(c_program->arguments[1]);
            delete_job(num_job);
            now_job = -1;
            return 0;
        }
        
        if (!strcmp(c_program->name, "mcat"))
        {
            if (c_program->number_of_arguments == 2)
                mcat(NULL);
            
            if (c_program->number_of_arguments == 3)
                mcat(c_program->arguments[1]);
            
            delete_job(num_job);
            now_job = -1;
            return 0;
        }
        
        if (!strcmp(c_program->name, "msed"))
        {
            if (c_program->number_of_arguments != 4)
            {
                now_job = -1;
                return 0;
            }
            
            msed(c_program->arguments[1], c_program->arguments[2]);
            
            delete_job(num_job);
            now_job = -1;
            return 0;
        }
        
        if (!strcmp(c_program->name, "mgrep"))
        {
            if ((c_program->number_of_arguments > 4) && (c_program->number_of_arguments <= 2))
            {
                now_job = -1;
                delete_job(num_job);
                return 0;
            }
            if (c_program->number_of_arguments == 3)
            {
                mgrep(c_program->arguments[1], 0);
                now_job = -1;
                delete_job(num_job);
                return 0;
            }
            if (c_program->number_of_arguments == 4)
            {
                if (!strcmp(c_program->arguments[3], "-v"))
                {
                    mgrep(c_program->arguments[1], 1);
                    now_job = -1;
                    delete_job(num_job);
                    return 0;
                }
                now_job = -1;
                delete_job(num_job);
                return 0;
            }
        }
        
        c_pid = fork();
        c_program->pid = c_pid;
        
        if (!c_pid)
        {
            signal(SIGTTIN,SIG_DFL);
            setpgid(getpid(), getppid());
            
            if (c_program->input_file!=NULL)
            {
                if ((fd1[0]=open(c_program->input_file,O_RDONLY))==-1)
                {
                    perror("open");
                    return -1;
                }
                dup2(fd1[0],0);
                close(fd1[0]);
            }
            
            if (c_program->output_file!=NULL)
            {
                if (c_program->output_type==1)
                {
                    if ((fd1[1]=open(c_program->output_file,O_CREAT|O_WRONLY,0666))==-1)
                    {
                        perror("open");
                        return -1;
                    }
                }
                else
                    if ((fd1[1]=open(c_program->output_file,O_CREAT|O_WRONLY|O_APPEND,0666))==-1)
                    {
                        perror("open");
                        return -1;
                    }
                dup2(fd1[1],1);
                close(fd1[1]);
            }
            
            if (!strcmp(c_program->name, "jobs"))
            {
                print_jobs();
                free_mas_of_jobs();
                free_globals();
                exit(0);
            }
            
            execvp(c_program->name, c_program->arguments);
            perror("exec");
            free_mas_of_jobs();
            free_globals();
            exit(-1);
        }
        
    }
    
    else
    {
        pipe(fd1);
        for (i = 0; i < mas_of_jobs[num_job].number_programs; i++)
        {
            c_program = &mas_of_jobs[num_job].mas_programs[i];
            c_program->arguments = realloc(c_program->arguments, sizeof(char *) * (c_program->number_of_arguments + 1));
            c_program->arguments[c_program->number_of_arguments] = (char *) 0;
            c_program->number_of_arguments++;
            
            pipe(fd2);
            c_pid = fork();
            
            c_program->pid = c_pid;
            
            if (!c_pid)
            {
                signal(SIGTTIN,SIG_DFL);
                setpgid(getpid(), getppid());
                
                if (!i)
                {
                    dup2(fd2[1],1);
                }
                else if (i < mas_of_jobs[number_jobs-1].number_programs - 1)
                {
                    dup2(fd1[0],0);
                    dup2(fd2[1],1);
                }
                else
                {
                    dup2(fd1[0],0);
                }
                
                close(fd1[0]);
                close(fd1[1]);
                close(fd2[0]);
                close(fd2[1]);
                
                if (c_program->input_file!=NULL)
                {
                    if ((fd1[0]=open(c_program->input_file,O_RDONLY)) == -1)
                    {
                        perror("open");
                        return -1;
                    }
                    dup2(fd1[0],0);
                    close(fd1[0]);
                }
                
                if (c_program->output_file!=NULL)
                {
                    if (c_program->output_type==1)
                    {
                        if ((fd1[1]=open(c_program->output_file,O_CREAT|O_WRONLY,0666))==-1)
                        {
                            perror("open");
                            return -1;
                        }
                    }
                    else
                        if ((fd1[1]=open(c_program->output_file,O_CREAT|O_WRONLY|O_APPEND,0666))==-1)
                        {
                            perror("open");
                            return -1;
                        }
                    dup2(fd1[1],1);
                    close(fd2[1]);
                }
                
                if (!strcmp(c_program->name, "jobs"))
                {
                    delete_job(num_job);
                    now_job = -1;
                    free_mas_of_jobs();
                    free_globals();
                    exit(0);
                }
                
                if (!strcmp(c_program->name, "mcat"))
                {
                    if (c_program->number_of_arguments == 2)
                        mcat(NULL);
                    
                    if (c_program->number_of_arguments == 3)
                        mcat(c_program->arguments[1]);
                    
                    delete_job(num_job);
                    now_job = -1;
                    free_mas_of_jobs();
                    free_globals();
                    exit(0);
                }
                
                if (!strcmp(c_program->name, "msed"))
                {
                    if (c_program->number_of_arguments != 4)
                    {
                        now_job = -1;
                        free_mas_of_jobs();
                        free_globals();
                        exit(0);
                    }
                    
                    msed(c_program->arguments[1], c_program->arguments[2]);
                    
                    delete_job(num_job);
                    now_job = -1;
                    free_mas_of_jobs();
                    free_globals();
                    exit(0);
                }
                
                if (!strcmp(c_program->name, "mgrep"))
                {
                    if ((c_program->number_of_arguments > 4) && (c_program->number_of_arguments <= 2))
                    {
                        now_job = -1;
                        delete_job(num_job);
                        free_mas_of_jobs();
                        free_globals();
                        exit(0);
                    }
                    if (c_program->number_of_arguments == 3)
                    {
                        mgrep(c_program->arguments[1], 0);
                        now_job = -1;
                        delete_job(num_job);
                        free_mas_of_jobs();
                        free_globals();
                        exit(0);
                    }
                    if (c_program->number_of_arguments == 4)
                    {
                        if (!strcmp(c_program->arguments[3], "-v"))
                        {
                            mgrep(c_program->arguments[1], 1);
                            now_job = -1;
                            delete_job(num_job);
                            return 0;
                        }
                        now_job = -1;
                        delete_job(num_job);
                        free_mas_of_jobs();
                        free_globals();
                        exit(0);
                    }
                }
                
                execvp(c_program->name, c_program->arguments);
                perror("exec");
                free_mas_of_jobs();
                free_globals();
            }
            close(fd1[0]);
            close(fd1[1]);
            fd1[0]=fd2[0];
            fd1[1]=fd2[1];
        }
        close(fd1[0]);
        close(fd1[1]);
    }
    
    signal(SIGINT,ctrl_c_pushed);
    signal(SIGTSTP,ctrl_z_pushed);
    
    errno  = 0;
    while ((pid = waitpid(-1, NULL, WUNTRACED)))
    {
        if (errno == ECHILD)
        {
            break;
        }
    }
    
    signal(SIGINT,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);
    
    free_mas_of_jobs();
    free_globals();
    tcsetpgrp(0, getpgid(getppid()));
    
    exit(0);
    
}

int add_job_bg(struct job job)
{
    struct job *tmp;
    tmp = realloc(mas_of_jobs_in_bg, sizeof(struct job) * (number_jobs_in_bg + 1));
    
    if (tmp == NULL)
    {
        printf("Ошибка при выделении памяти");
        /*Что-то еще*/
    }
    
    mas_of_jobs_in_bg = tmp;
    mas_of_jobs_in_bg[number_jobs_in_bg].mas_programs = job.mas_programs;
    mas_of_jobs_in_bg[number_jobs_in_bg].number_programs = job.number_programs;
    mas_of_jobs_in_bg[number_jobs_in_bg].status = job.status;
    mas_of_jobs_in_bg[number_jobs_in_bg].pid_helper = job.pid_helper;
    number_jobs_in_bg++;
    
    job_man_bg(&mas_of_jobs_in_bg[number_jobs_in_bg-1]);
    
    return 0;
}

int add_job(struct job job)
{
    if (job.status == 1)
        return add_job_bg(job);
    
    struct job *tmp;
    tmp = realloc(mas_of_jobs, sizeof(struct job) * (number_jobs + 1));
    
    if (tmp == NULL)
    {
        printf("Ошибка при выделении памяти");
        /*Что-то еще*/
    }
    
    mas_of_jobs = tmp;
    mas_of_jobs[number_jobs].mas_programs = job.mas_programs;
    mas_of_jobs[number_jobs].number_programs = job.number_programs;
    mas_of_jobs[number_jobs].status = job.status;
    mas_of_jobs[number_jobs].pid_helper = job.pid_helper;
    number_jobs++;
    
    return 0;
}

void free_job(struct job *c_job)
{
    struct program *c_program;
    int j, k;
    
    for (j = 0; j < c_job->number_programs; j++)
    {
        c_program = &c_job->mas_programs[j];
        free(c_program->output_file);
        free(c_program->name);
        free(c_program->input_file);
        for (k = 0; k < c_program->number_of_arguments; k++)
        {
            free(c_program->arguments[k]);
        }
        free(c_program->arguments);
    }
    free(c_job->mas_programs);
    free(c_job);
}

int find_job()
{
    int i;
    for (i = 0; i < number_jobs; i++)
    {
        if (mas_of_jobs[i].status == 0)
            return i;
    }
    return -1;
}

void child_proc_ended(int sig)
{
    int i, status = 0;
    for(i = 0; i < number_jobs_in_bg; i++)
    {
        if(waitpid(mas_of_jobs_in_bg[i].pid_helper, &status, WNOHANG|WUNTRACED))
        {
            if (WIFSTOPPED(status) && (mas_of_jobs_in_bg[i].status != 3))
            {
                fprintf(stderr, "[GROUP OF PROCESSES WITH GPID %i STOPPED]\n", mas_of_jobs_in_bg[i].pid_helper);
                mas_of_jobs_in_bg[i].status = 3;
                add_job(mas_of_jobs_in_bg[i]);
                delete_job_in_mas_of_bg(i);
                signal(SIGCHLD, child_proc_ended);
                return;
            }
            else
            {
                
                fprintf(stderr, "[GROUP OF PROCESSES WITH GPID %i FINISHED]\n", mas_of_jobs_in_bg[i].pid_helper);
                /*print_job(&mas_of_jobs[i]);*/
                delete_job_in_mas_of_bg(i);
                signal(SIGCHLD, child_proc_ended);
                return;
            }
        }
    }
    
    for(i = 0; i < number_jobs; i++)
    {
        if(waitpid(mas_of_jobs[i].pid_helper, &status, WNOHANG|WUNTRACED))
        {
            if (WIFSTOPPED(status) && (mas_of_jobs[i].status != 3))
            {
                fprintf(stderr, "[GROUP OF PROCESSES WITH GPID %i STOPPED]\n", mas_of_jobs[i].pid_helper);
                mas_of_jobs[i].status = 3;
                signal(SIGCHLD, child_proc_ended);
                return;
            }
            else
            {
                fprintf(stderr, "[GROUP OF PROCESSES WITH GPID %i FINISHED]\n", mas_of_jobs[i].pid_helper);
                /*print_job(&mas_of_jobs[i]);*/
                delete_job(i);
                flag++;
                signal(SIGCHLD, child_proc_ended);
                return;
            }
        }
    }
    signal(SIGCHLD, child_proc_ended);
}