#ifndef job_struct_h
#define job_struct_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <pwd.h>
#include <errno.h>

#include "program_struct.h"
#include "global_var.h"
#include "Side_Functions.h"

struct job
{
    int status; /* 0 = ожидает начало работы, 1 = работает в BG, 2 = работает в FG, 3 = находится в паузе*/
    struct program *mas_programs;
    int number_programs;
    pid_t pid_helper;
    int is_notified_about_stop;/* Актуально только для процессов в BG. Равняется 1 если уже было уведомление об остановкек процесса*/
};

int job_man();

int add_job(struct job job);

int free_mas_of_jobs();

void free_job(struct job *c_job);

int find_job();

void child_proc_ended(int sig);

int job_man_bg(struct job *job);

int initializationOfGlobalVariables(int argc, char *argv[]);

#endif
