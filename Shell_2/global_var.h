#ifndef global_var_h
#define global_var_h

#include <stdlib.h>
#include <stdio.h>

extern struct environmentVariables globalVariables;

struct environmentVariables
{
    char **argv;
    int argc;
    int statusOfLastProcess;
    char *username;
    char *homeDirectory;
    char *shellDirectory;
    int UID;
    char *currentDirectory;
    int PID;
};

#endif
