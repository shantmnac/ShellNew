#ifndef Side_Functions_h
#define Side_Functions_h

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

int mgrep(char *s, int v_flag);

int mcat(char *str);

int msed(char *replacementString, char *replacedByAString);

#endif
