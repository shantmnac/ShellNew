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

struct environmentVariables globalVariables;