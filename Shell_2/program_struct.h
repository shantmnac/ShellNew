#ifndef program_struct_h
#define program_struct_h

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

struct program
{
    pid_t pid;
    char *name;
    int number_of_arguments;
    char **arguments;
    char *input_file , *output_file;
    int output_type; /* 1 = перезаписать, 2 = дописать */
};

#endif
