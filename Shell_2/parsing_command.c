#include "parsing_command.h"

void memmoryAllarm(char *callerFunction)
{
    fprintf(stderr, "%s: memmory allocation issue\n", callerFunction);
}

void print_command(char *s, int size)
{
    int i = 0;
    for (i = 0; i < size; i++)
    {
        putchar(s[i]);
    }
}

char *reverse(char *s)
{
    int i, j;
    char c;
    
    for (i = 0, j = strlen(s) - 1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
    return s;
}

char *itoa (int n)
{
    char *t = NULL;
    int z = 0, j = 0;
    
    if (n < 0) {
        z=1;
        n=-n;
    }
    
    do{
        t = (char*)realloc(t, j + 1);
        t[j] = n%10 + '0';
        j++;
    }while (n/=10);
    
    if (z){
        t = (char*)realloc(t, j + 1);
        t[j] = '-';
        j++;
    }
    
    t = (char*)realloc(t, j + 1);
    t[j] = '\0';
    return reverse(t);
}

char *variableSubstitution(char **currentWord, int variablePosition, int *variablesSize){
    char tempChar, *variableName = NULL, *replacementString = NULL, *tmpVariableValue = NULL;
    int variableNameSize = 0, numOfArgv = 0, isVariableCorrect = 0, j = 0, sizeOfVariableValue = 0;
    
    *variablesSize = variablePosition;
    variablePosition++;
    
    if ((*currentWord)[variablePosition] == '{') {
        variablePosition++;
        while ((*currentWord)[variablePosition] != '}') {
            if ((*currentWord)[variablePosition] == '\n') {
                fprintf(stderr, "variableSubstitution: expected bracket\n");
                if (variableName != NULL) {
                    free(variableName);
                }
                return NULL;
            }
            if (!variableNameSize) {
                variableName = (char*)malloc(1);
            }
            else{
                variableName = (char*)realloc(variableName, variableNameSize + 1);
            }
            if (variableName == NULL) {
                memmoryAllarm("readCurrentWord");
            }
            variableName[variableNameSize] = tolower((*currentWord)[variablePosition]);
            variableNameSize++;
            variablePosition++;
        }
        
        variableName = (char*)realloc(variableName, variableNameSize + 1);
        if (variableName == NULL) {
            memmoryAllarm("readCurrentWord");
        }
        variableName[variableNameSize] = '\0';
        
        
        if (!strcmp(variableName, "user")) {
            replacementString = globalVariables.username;
            isVariableCorrect = 1;
        }
        if (!strcmp(variableName, "home")) {
            replacementString = globalVariables.homeDirectory;
            isVariableCorrect = 1;
        }
        if (!strcmp(variableName, "shell")) {
            replacementString = globalVariables.shellDirectory;
            isVariableCorrect = 1;
        }
        if (!strcmp(variableName, "uid")) {
            replacementString = itoa(globalVariables.UID);
            isVariableCorrect = 1;
        }
        if (!strcmp(variableName, "pwd")) {
            replacementString = globalVariables.currentDirectory;
            isVariableCorrect = 1;
        }
        if (!strcmp(variableName, "pid")) {
            replacementString = itoa(globalVariables.PID);
            isVariableCorrect = 1;
        }
        
        if (!isVariableCorrect) {
            fprintf(stderr, "variableSubstitution: incorrect variable\n");
            if (variableName != NULL) {
                free(variableName);
            }
            return NULL;
        }
    }
    else{
        switch (tempChar) {
                
            case '?':
                replacementString = itoa(globalVariables.statusOfLastProcess);
                break;
                
            case '#':
                replacementString = itoa(globalVariables.argc);
                break;
                
            default:
                if(isdigit((*currentWord)[variablePosition])){
                    numOfArgv = atoi(&(*currentWord)[variablePosition]);
                    variablePosition++;
                    while ((*currentWord)[variablePosition] != ' ') {
                        if ((*currentWord)[variablePosition] == '\n') {
                            variablePosition--;
                            break;
                        }
                        else{
                            if (isdigit((*currentWord)[variablePosition])) {
                                numOfArgv = numOfArgv * 10 + atoi(&tempChar);
                            }
                            else{
                                fprintf(stderr, "variableSubstitution: expected digit\n");
                                return NULL;
                            }
                        }
                    }
                    
                    if (numOfArgv + 1 > globalVariables.argc) {
                        fprintf(stderr, "variableSubstitution: incorrect variable\n");
                        return NULL;
                    }
                    else{
                        replacementString = globalVariables.argv[numOfArgv];
                    }
                }
                else {
                    fprintf(stderr, "variableSubstitution: expected digit\n");
                    return NULL;
                }
                
                break;
        }
    }
    free(variableName);
    *variablesSize = variablePosition - *variablesSize + 1;
    
    while (replacementString[j] != '\0') {
        
        if (!sizeOfVariableValue) {
            tmpVariableValue = (char*)malloc(sizeOfVariableValue + 1);
        }
        else{
            tmpVariableValue = (char*)realloc(tmpVariableValue, sizeOfVariableValue + 1);
        }
        if (tmpVariableValue == NULL) {
            memmoryAllarm("readCurrentWord");
        }
        
        tmpVariableValue[sizeOfVariableValue] = replacementString[j];
        sizeOfVariableValue++;
        j++;
    }
    
    return tmpVariableValue;
}

char *replaceVariablesInStr(char **currentWord){
    char *tempStr = NULL, *variablesValue = NULL;
    int tempStrSize = 0, quotes = 0, variablesSize = 0, i = 0, j = 0;;
    
    while (1) {
        switch ((*currentWord)[i]) {
            case '\'':
                i++;
                while ((*currentWord)[i] != '\'') {
                    if ((*currentWord)[i] != '\0') {
                        return 0;
                    }
                    i++;
                    break;
                }
                continue;
                
            case '"':
                if (!tempStrSize) {
                    tempStr = (char*)malloc(tempStrSize + 1);
                }
                else{
                    tempStr = (char*)realloc(tempStr, tempStrSize + 1);
                }
                
                if (tempStr == NULL) {
                    memmoryAllarm("readCurrentWord");
                }
                
                tempStr[tempStrSize] = (*currentWord)[i];
                tempStrSize++;
                i++;
                quotes = 1;
                break;
                
            default:
                quotes = 0;
                break;
        }
        
        if ((*currentWord)[i] == '\0') {
            break;
        }
        
        switch (quotes) {
            case 0:{
                while ((*currentWord)[i] != ' ') {
                    if ((*currentWord)[i] == '\0') {
                        break;
                    }
                    if ((*currentWord)[i] == '$') {
                        variablesValue = variableSubstitution(currentWord, i, &variablesSize);
                        if (variablesValue != NULL) {
                            if (!tempStrSize) {
                                tempStr = (char*)malloc(tempStrSize + 1);
                            }
                            else{
                                tempStr = (char*)realloc(tempStr, tempStrSize + 1);
                            }
                            
                            if (tempStr == NULL) {
                                memmoryAllarm("readCurrentWord");
                            }
                            
                            while (variablesValue[j] != '\0') {
                                if (!tempStrSize) {
                                    tempStr = (char*)malloc(tempStrSize + 1);
                                }
                                else{
                                    tempStr = (char*)realloc(tempStr, tempStrSize + 1);
                                }
                                
                                if (tempStr == NULL) {
                                    memmoryAllarm("readCurrentWord");
                                }
                                
                                tempStr[tempStrSize] = variablesValue[j];
                                tempStrSize++;
                                j++;
                            }
                            free(variablesValue);
                            i += variablesSize;
                            variablesSize = 0;
                            j = 0;
                            continue;
                        }
                    }
                    
                    if (!tempStrSize) {
                        tempStr = (char*)malloc(tempStrSize + 1);
                    }
                    else{
                        tempStr = (char*)realloc(tempStr, tempStrSize + 1);
                    }
                    
                    if (tempStr == NULL) {
                        memmoryAllarm("readCurrentWord");
                    }
                    
                    tempStr[tempStrSize] = (*currentWord)[i];
                    tempStrSize++;
                    i++;
                }
                
                if ((*currentWord)[i] == ' ') {
                    if (!tempStrSize) {
                        tempStr = (char*)malloc(tempStrSize + 1);
                    }
                    else{
                        tempStr = (char*)realloc(tempStr, tempStrSize + 1);
                    }
                    
                    if (tempStr == NULL) {
                        memmoryAllarm("readCurrentWord");
                    }
                    
                    tempStr[tempStrSize] = (*currentWord)[i];
                    tempStrSize++;
                    i++;
                }
                break;
            }
                
            case 1:{
                while ((*currentWord)[i] != '"') {
                    if ((*currentWord)[i] == '\0') {
                        break;
                    }
                    if ((*currentWord)[i] == '$') {
                        variablesValue = variableSubstitution(currentWord, i, &variablesSize);
                        if (variablesValue != NULL) {
                            if (!tempStrSize) {
                                tempStr = (char*)malloc(tempStrSize + 1);
                            }
                            else{
                                tempStr = (char*)realloc(tempStr, tempStrSize + 1);
                            }
                            
                            if (tempStr == NULL) {
                                memmoryAllarm("readCurrentWord");
                            }
                            
                            while (variablesValue[j] != '\0') {
                                if (!tempStrSize) {
                                    tempStr = (char*)malloc(tempStrSize + 1);
                                }
                                else{
                                    tempStr = (char*)realloc(tempStr, tempStrSize + 1);
                                }
                                
                                if (tempStr == NULL) {
                                    memmoryAllarm("readCurrentWord");
                                }
                                
                                tempStr[tempStrSize] = variablesValue[j];
                                tempStrSize++;
                                j++;
                            }
                            free(variablesValue);
                            i += variablesSize;
                            variablesSize = 0;
                            j = 0;
                            continue;
                        }
                    }
                    
                    if (!tempStrSize) {
                        tempStr = (char*)malloc(tempStrSize + 1);
                    }
                    else{
                        tempStr = (char*)realloc(tempStr, tempStrSize + 1);
                    }
                    
                    if (tempStr == NULL) {
                        memmoryAllarm("readCurrentWord");
                    }
                    
                    tempStr[tempStrSize] = (*currentWord)[i];
                    tempStrSize++;
                    i++;
                }
                
                if ((*currentWord)[i] == '"') {
                    if (!tempStrSize) {
                        tempStr = (char*)malloc(tempStrSize + 1);
                    }
                    else{
                        tempStr = (char*)realloc(tempStr, tempStrSize + 1);
                    }
                    
                    if (tempStr == NULL) {
                        memmoryAllarm("readCurrentWord");
                    }
                    
                    tempStr[tempStrSize] = (*currentWord)[i];
                    tempStrSize++;
                    i++;
                }
                break;
            }
                
            default:
                break;
        }
        
    }
    
    if (tempStrSize) {
        tempStr = (char*)realloc(tempStr, tempStrSize + 1);
        if (tempStr == NULL) {
            memmoryAllarm("readCurrentWord");
        }
        tempStr[tempStrSize] = '\0';
    }
    
    return tempStr;
}


char * my_strcpy(char *s1)
{
    int i = 0, size1;
    char *p = NULL;
    size1 = (int) strlen(s1);
    for(i = 0; i < size1 + 1; i++)
    {
        p = realloc(p, sizeof(char) * (i+1));
        p[i] = s1[i];
    }
    return p;
}

int read_command(struct job *c_job)
{
    char *name, *tmp1 = NULL, *tmp4 = NULL, c = -1, **parameters, **tmp2 = NULL, *output, *input;
    int name_size = 0, number_parameters = 0, param_size = 0, number_programs = 0, output_size, input_size, i, j;
    struct program *programs = NULL, *tmp3 = NULL, c_program;
    
    while (c == -1)
        c = getchar();
    while (1)
    {
        tmp3 = realloc(programs, sizeof(struct program) * (number_programs + 1));
        if (tmp3 == NULL)
        {
            printf("Ошибка при выделении памяти");
            /*Что то еще*/
        }
        programs = tmp3;
        number_programs++;
        
        c_program.output_type = -1;
        c_program.output_file = NULL;
        c_program.number_of_arguments = 0;
        c_program.input_file = NULL;
        c_program.arguments = NULL;
        c_program.name =NULL;
        c_program.pid = -1;
        
        /*    Считывание имени     */
        
        name = NULL;
        name_size = 0;
        while ((c == ' ') || (c == '\t'))
        {
            c = getchar();
            if (c == '\\')
            {
                c = getchar();
                if (c != '\n')
                {
                    ungetc('\\', stdin);
                    ungetc(c, stdin);
                    break;
                }
                c = getchar();
            }
        }
        while ((c != '\n') && (c != ' ') && (c != ';') && (c!= '\t') && (c != '|') && (c != '>') && ( c != '<') && (c != '#'))
        {
            if (c == '\\')
            {
                c = getchar();
                switch (c)
                {
                    case '\n':
                        break;
                        
                    default:
                        tmp1 = realloc(name, (name_size+1) * sizeof(char));
                        if (tmp1 == NULL)
                        {
                            printf("Ошибка при выделении памяти");
                            /*Что-то еще*/
                        }
                        name = tmp1;
                        name_size++;
                        name[name_size-1] = c;
                        break;
                }
                c = getchar();
            }
            else
            {
                tmp1 = realloc(name, name_size+1);
                if (tmp1 == NULL)
                {
                    printf("Ошибка при выделении памяти");
                    /*Что-то еще*/
                }
                name = tmp1;
                name_size++;
                name[name_size-1] = c;
                c = getchar();
            }
        }
        
        tmp1 = realloc(name, sizeof(char) * (name_size + 1));
        if (tmp1 == NULL)
        {
            printf("Ошибка при выделении памяти");
            /*Что то еще*/
        }
        name = tmp1;
        name[name_size] = '\0';
        name_size++;
        
        /*    Считывание параметров    */
        
        parameters = malloc(sizeof(char *));
        parameters[0] =  my_strcpy(name);
        number_parameters = 1;
        
        while ((c == ' ') || (c == '\t'))
        {
            c = getchar();
            if (c == '\\')
            {
                c = getchar();
                if (c != '\n')
                {
                    ungetc('\\', stdin);
                    ungetc(c, stdin);
                    break;
                }
                c = getchar();
            }
        }
        while ((c != '\n') && (c != ';') && (c!= '\t') && (c!= '|') && (c != '<') && (c!= '>') && (c != '&') && (c != '#'))
        {
            tmp2 = realloc(parameters, sizeof(char*) * (number_parameters + 1));
            if (tmp2 == NULL)
            {
                printf("Ошибка при выделении памяти");
                /*Что-то еще*/
            }
            parameters = tmp2;
            number_parameters++;
            parameters[number_parameters-1] = NULL;
            param_size = 0;
            
            while ((c == ' ') || (c == '\t'))
            {
                c = getchar();
                if (c == '\\')
                {
                    c = getchar();
                    if (c != '\n')
                    {
                        ungetc('\\', stdin);
                        ungetc(c, stdin);
                        break;
                    }
                    c = getchar();
                }
            }
            
            if (c == '"')
            {
                c = getchar();
                while (c != '"')
                {
                    if (c == '\\')
                    {
                        c = getchar();
                        
                        switch (c)
                        {
                            case '\n':
                                break;
                                
                            default:
                                tmp1 = realloc(parameters[number_parameters-1], sizeof(char) * (param_size+1));
                                if (tmp1 == NULL)
                                {
                                    printf("Ошибка при выделении памяти");
                                    /*Что-то еще*/
                                }
                                parameters[number_parameters-1] = tmp1;
                                param_size++;
                                parameters[number_parameters-1][param_size-1] = c;
                                break;
                        }
                        
                        c = getchar();
                        
                    }
                    else
                    {
                        tmp1 = realloc(parameters[number_parameters-1], sizeof(char) * (param_size+1));
                        if (tmp1 == NULL)
                        {
                            printf("Ошибка при выделении памяти");
                            /*Что-то еще*/
                        }
                        parameters[number_parameters-1] = tmp1;
                        param_size++;
                        parameters[number_parameters-1][param_size-1] = c;
                        c = getchar();
                    }
                }
                
                tmp1 = realloc(parameters[number_parameters-1], sizeof(char) * (param_size + 1));
                if (tmp1 == NULL)
                {
                    printf("Ошибка при выделении памяти");
                    /*Что то еще*/
                }
                parameters[number_parameters-1] = tmp1;
                parameters[number_parameters-1][param_size] = '\0';
                
                c = getchar();
            }
            else if (c == '\'')
            {
                c = getchar();
                while (c != '\'')
                {
                    if (c == '\\')
                    {
                        c = getchar();
                        
                        switch (c)
                        {
                            case '\n':
                                break;
                                
                            default:
                                tmp1 = realloc(parameters[number_parameters-1], sizeof(char) * (param_size+1));
                                if (tmp1 == NULL)
                                {
                                    printf("Ошибка при выделении памяти");
                                    /*Что-то еще*/
                                }
                                parameters[number_parameters-1] = tmp1;
                                param_size++;
                                parameters[number_parameters-1][param_size-1] = c;
                                break;
                        }
                        c = getchar();
                        
                    }
                    else
                    {
                        tmp1 = realloc(parameters[number_parameters-1], sizeof(char) * (param_size+1));
                        if (tmp1 == NULL)
                        {
                            printf("Ошибка при выделении памяти");
                            /*Что-то еще*/
                        }
                        parameters[number_parameters-1] = tmp1;
                        param_size++;
                        parameters[number_parameters-1][param_size-1] = c;
                        c = getchar();
                    }
                }
                
                tmp1 = realloc(parameters[number_parameters-1], sizeof(char) * (param_size + 1));
                if (tmp1 == NULL)
                {
                    printf("Ошибка при выделении памяти");
                    /*Что то еще*/
                }
                parameters[number_parameters-1] = tmp1;
                parameters[number_parameters-1][param_size] = '\0';
                
                tmp4 = replaceVariablesInStr(&parameters[number_parameters-1]);
                
                free(parameters[number_parameters-1]);
                parameters[number_parameters-1] = tmp4;
                
                c = getchar();
            }
            else
            {
                while ((c != ' ') && (c != '\n') && (c != '\t'))
                {
                    if (c == '\\')
                    {
                        c = getchar();
                        
                        switch (c)
                        {
                            case '\n':
                                break;
                                
                            default:
                                tmp1 = realloc(parameters[number_parameters-1], sizeof(char) * (param_size+1));
                                if (tmp1 == NULL)
                                {
                                    printf("Ошибка при выделении памяти");
                                    /*Что-то еще*/
                                }
                                parameters[number_parameters-1] = tmp1;
                                param_size++;
                                parameters[number_parameters-1][param_size-1] = c;
                                break;
                        }
                        
                        c = getchar();
                        
                    }
                    else
                    {
                        tmp1 = realloc(parameters[number_parameters-1], sizeof(char) * (param_size+1));
                        if (tmp1 == NULL)
                        {
                            printf("Ошибка при выделении памяти");
                            /*Что-то еще*/
                        }
                        parameters[number_parameters-1] = tmp1;
                        param_size++;
                        parameters[number_parameters-1][param_size-1] = c;
                        c = getchar();
                    }
                }
                
                tmp1 = realloc(parameters[number_parameters-1], sizeof(char) * (param_size + 1));
                if (tmp1 == NULL)
                {
                    printf("Ошибка при выделении памяти");
                    /*Что то еще*/
                }
                parameters[number_parameters-1] = tmp1;
                parameters[number_parameters-1][param_size] = '\0';
            }
            
            while ((c == ' ') || (c == '\t'))
            {
                c = getchar();
                if (c == '\\')
                {
                    c = getchar();
                    if (c != '\n')
                    {
                        ungetc('\\', stdin);
                        ungetc(c, stdin);
                        break;
                    }
                    c = getchar();
                }
            }
        }
        
        /*    Считывание параметров запуска процесса    */
        
        output = NULL;
        input = NULL;
        output_size = 0;
        input_size = 0;
        while ((c != '|') && (c != '#') && (c!= ';') && (c != '\n'))
        {
            while ((c == ' ') || (c == '\t'))
            {
                c = getchar();
                if (c == '\\')
                {
                    c = getchar();
                    if (c != '\n')
                    {
                        ungetc('\\', stdin);
                        ungetc(c, stdin);
                        break;
                    }
                    c = getchar();
                }
            }
            
            if (c == '>')
            {
                c = getchar();
                if (c == '>')
                {
                    c = getchar();
                    while ((c == ' ') || (c == '\t'))
                    {
                        c = getchar();
                        if (c == '\\')
                        {
                            c = getchar();
                            if (c != '\n')
                            {
                                ungetc('\\', stdin);
                                ungetc(c, stdin);
                                break;
                            }
                            c = getchar();
                        }
                    }
                    
                    while ((c != ' ') && (c != '\t') && (c != '\n') && (c != ';'))
                    {
                        if (c == '\\')
                        {
                            c = getchar();
                            switch (c)
                            {
                                case '\n':
                                    break;
                                    
                                default:
                                    tmp1 = realloc(output, (output_size+1) * sizeof(char));
                                    if (tmp1 == NULL)
                                    {
                                        printf("Ошибка при выделении памяти");
                                        /*Что-то еще*/
                                    }
                                    output = tmp1;
                                    output_size++;
                                    output[output_size-1] = c;
                                    break;
                            }
                            c = getchar();
                        }
                        else
                        {
                            tmp1 = realloc(output, output_size+1);
                            if (tmp1 == NULL)
                            {
                                printf("Ошибка при выделении памяти");
                                /*Что-то еще*/
                            }
                            output = tmp1;
                            output_size++;
                            output[output_size-1] = c;
                            c = getchar();
                        }
                    }
                    
                    tmp1 = realloc(output, sizeof(char) * (output_size + 1));
                    if (tmp1 == NULL)
                    {
                        printf("Ошибка при выделении памяти");
                        /*Что то еще*/
                    }
                    output = tmp1;
                    output[output_size] = '\0';
                    
                    c_program.output_type = 2;
                    c_program.output_file = output;
                    
                    while ((c == ' ') || (c == '\t'))
                    {
                        c = getchar();
                        if (c == '\\')
                        {
                            c = getchar();
                            if (c != '\n')
                            {
                                ungetc('\\', stdin);
                                ungetc(c, stdin);
                                break;
                            }
                            c = getchar();
                        }
                    }
                    
                    continue;
                }
                else
                {
                    while ((c == ' ') || (c == '\t'))
                    {
                        c = getchar();
                        if (c == '\\')
                        {
                            c = getchar();
                            if (c != '\n')
                            {
                                ungetc('\\', stdin);
                                ungetc(c, stdin);
                                break;
                            }
                            c = getchar();
                        }
                    }
                    
                    while ((c != ' ') && (c != '\t') && (c != '\n'))
                    {
                        if (c == '\\')
                        {
                            c = getchar();
                            switch (c)
                            {
                                case '\n':
                                    break;
                                    
                                default:
                                    tmp1 = realloc(output, (output_size+1) * sizeof(char));
                                    if (tmp1 == NULL)
                                    {
                                        printf("Ошибка при выделении памяти");
                                        /*Что-то еще*/
                                    }
                                    output = tmp1;
                                    output_size++;
                                    output[output_size-1] = c;
                                    break;
                            }
                            c = getchar();
                        }
                        else
                        {
                            tmp1 = realloc(output, output_size+1);
                            if (tmp1 == NULL)
                            {
                                printf("Ошибка при выделении памяти");
                                /*Что-то еще*/
                            }
                            output = tmp1;
                            output_size++;
                            output[output_size-1] = c;
                            c = getchar();
                        }
                    }
                    
                    tmp1 = realloc(output, sizeof(char) * (output_size + 1));
                    if (tmp1 == NULL)
                    {
                        printf("Ошибка при выделении памяти");
                        /*Что то еще*/
                    }
                    output = tmp1;
                    output[output_size] = '\0';
                    
                    c_program.output_type = 1;
                    c_program.output_file = output;
                    
                    while ((c == ' ') || (c == '\t'))
                    {
                        c = getchar();
                        if (c == '\\')
                        {
                            c = getchar();
                            if (c != '\n')
                            {
                                ungetc('\\', stdin);
                                ungetc(c, stdin);
                                break;
                            }
                            c = getchar();
                        }
                    }
                    
                    continue;
                }
            }
            
            if (c == '<')
            {
                c = getchar();
                while ((c == ' ') || (c == '\t'))
                {
                    c = getchar();
                    if (c == '\\')
                    {
                        c = getchar();
                        if (c != '\n')
                        {
                            ungetc('\\', stdin);
                            ungetc(c, stdin);
                            break;
                        }
                        c = getchar();
                    }
                }
                
                while ((c != ' ') && (c != '\t') && (c != '\n'))
                {
                    if (c == '\\')
                    {
                        c = getchar();
                        switch (c)
                        {
                            case '\n':
                                break;
                                
                            default:
                                tmp1 = realloc(input, (input_size+1) * sizeof(char));
                                if (tmp1 == NULL)
                                {
                                    printf("Ошибка при выделении памяти");
                                    /*Что-то еще*/
                                }
                                input = tmp1;
                                input_size++;
                                input[input_size-1] = c;
                                break;
                        }
                        c = getchar();
                    }
                    else
                    {
                        tmp1 = realloc(input, input_size+1);
                        if (tmp1 == NULL)
                        {
                            printf("Ошибка при выделении памяти");
                            /*Что-то еще*/
                        }
                        input = tmp1;
                        input_size++;
                        input[input_size-1] = c;
                        c = getchar();
                    }
                }
                
                tmp1 = realloc(input, sizeof(char) * (input_size + 1));
                if (tmp1 == NULL)
                {
                    printf("Ошибка при выделении памяти");
                    /*Что то еще*/
                }
                input = tmp1;
                input[input_size] = '\0';
                c_program.input_file = input;
                
                while ((c == ' ') || (c == '\t'))
                {
                    c = getchar();
                    if (c == '\\')
                    {
                        c = getchar();
                        if (c != '\n')
                        {
                            ungetc('\\', stdin);
                            ungetc(c, stdin);
                            break;
                        }
                        c = getchar();
                    }
                }
                
                continue;
            }
            
            if (c == '&')
            {
                c = getchar();
                while ((c == ' ') || (c == '\t'))
                {
                    c = getchar();
                    if (c == '\\')
                    {
                        c = getchar();
                        if (c != '\n')
                        {
                            ungetc('\\', stdin);
                            ungetc(c, stdin);
                            break;
                        }
                        c = getchar();
                    }
                }
                
                if ((c != ';') && (c != '\n'))
                {
                    printf("Неверная команда\n");
                    while (c != '\n'){c = getchar();}
                    programs[number_programs-1] = c_program;
                    for (j = 0; j < number_programs; j++)
                    {
                        for (i = 0; i < number_parameters; i++)
                        {
                            free(programs[j].arguments[i]);
                        }
                        if (programs[j].arguments != NULL)
                            free(programs[j].arguments);
                        if (programs[j].input_file != NULL)
                            free(programs[j].input_file);
                        if (programs[j].output_file != NULL)
                            free(programs[j].output_file);
                        if (programs[j].name != NULL)
                            free(programs[j].name);
                    }
                    if (programs != NULL)
                        free(programs);
                    
                    while (c != '\n')
                        c= getchar();
                    
                    return -2;
                }
                
                c_program.name = name;
                c_program.arguments = parameters;
                c_program.number_of_arguments = number_parameters;
                if (c_program.output_type == -1)
                    c_program.output_type = 1;
                
                programs[number_programs-1] = c_program;
                
                c_job->mas_programs = programs;
                c_job->number_programs = number_programs;
                c_job->status = 1;
                
                return c;
            }
            
            /* После перенаправлений появился неправильный символ  */
            
            printf("Неверная команда\n");
            while (c != '\n'){c = getchar();}
            programs[number_programs-1] = c_program;
            for (j = 0; j < number_programs; j++)
            {
                for (i = 0; i < number_parameters; i++)
                {
                    free(programs[j].arguments[i]);
                }
                if (programs[j].arguments != NULL)
                    free(programs[j].arguments);
                if (programs[j].input_file != NULL)
                    free(programs[j].input_file);
                if (programs[j].output_file != NULL)
                    free(programs[j].output_file);
                if (programs[j].name != NULL)
                    free(programs[j].name);
            }
            if (programs != NULL)
                free(programs);
            
            while (c != '\n')
                c= getchar();
            
            return -2;
        }
        
        /*Реализация конвейра */
        
        while ((c == ' ') || (c == '\t'))
        {
            c = getchar();
            if (c == '\\')
            {
                c = getchar();
                if (c != '\n')
                {
                    ungetc('\\', stdin);
                    ungetc(c, stdin);
                    break;
                }
                c = getchar();
            }
        }
        
        if (c == '|')
        {
            c_program.name = name;
            c_program.arguments = parameters;
            c_program.number_of_arguments = number_parameters;
            if (c_program.output_type == -1)
                c_program.output_type = 1;
            
            programs[number_programs-1] = c_program;
            
            c = getchar();
            continue;
        }
        
        /*    Завершение обработки строки      */
        
        if (c == '#')
        {
            c = getchar();
            while (c != '\n')
                c= getchar();
            
            c_program.name = name;
            c_program.arguments = parameters;
            c_program.number_of_arguments = number_parameters;
            if (c_program.output_type == -1)
                c_program.output_type = 1;
            programs[number_programs-1] = c_program;
            
            c_job->mas_programs = programs;
            c_job->number_programs = number_programs;
            c_job->status = 0;
            
            return '\n';
        }
        
        if (c == '\n')
        {
            c_program.name = name;
            c_program.arguments = parameters;
            c_program.number_of_arguments = number_parameters;
            
            if (c_program.output_type == -1)
                c_program.output_type = 1;
            programs[number_programs-1] = c_program;
            
            c_job->mas_programs = programs;
            c_job->number_programs = number_programs;
            c_job->status = 0;
            
            return '\n';
        }
        
        if (c == ';')
        {
            c_program.name = name;
            c_program.arguments = parameters;
            c_program.number_of_arguments = number_parameters;
            if (c_program.output_type == -1)
                c_program.output_type = 1;
            programs[number_programs-1] = c_program;
            
            c_job->mas_programs = programs;
            c_job->number_programs = number_programs;
            c_job->status = 0;
            
            return ';';
        }
        
        if (c == EOF)
        {
            
        }
        
    }
    return -1;
}