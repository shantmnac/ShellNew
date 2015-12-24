#include "Side_Functions.h"

int msed(char *replacementString, char *replacedByAString)
{
    int i = 0, k = 0, replaceMod = 0;
    int opaEstChe = 0;
    char tempChar = '!', *tmpReplacementString = NULL, *tmpReplacedByAString = NULL, *buffer = NULL;
    
    if ((replacementString == NULL) || (replacedByAString == NULL)){
        fprintf(stderr, "\x1b[1;31mmsed: not enough arguments\n\x1b[1;31m");
        return -1;
    }
    
    if ((replacementString[i] == '"') || (replacementString[i] == '\'')) {
        while (replacementString[i] != '\0') {
            i++;
        }
        if ((replacementString[i - 1] == '"') || (replacementString[i - 1] == '\'')){
            tmpReplacementString = (char*)malloc(i - 1);
            i = 1;
            while ((replacementString[i] != '"') && (replacementString[i] != '\'')){
                tmpReplacementString[k] = replacementString[i];
                k++;
                i++;
            }
            tmpReplacementString[k] = '\0';
        }
        else{
            tmpReplacementString = replacementString;
        }
    }
    else{
        tmpReplacementString = replacementString;
    }
    
    k = 0;
    i = 0;
    
    if ((replacedByAString[i] == '"') || (replacedByAString[i] == '\'')) {
        while (replacedByAString[i] != '\0') {
            i++;
        }
        if ((replacedByAString[i - 1] == '"') || (replacedByAString[i - 1] == '\'')){
            tmpReplacedByAString = (char*)malloc(i - 1);
            i = 1;
            while ((replacedByAString[i] != '"') && (replacedByAString[i] != '\'')){
                tmpReplacedByAString[k] = replacedByAString[i];
                k++;
                i++;
            }
            tmpReplacedByAString[k] = '\0';
        }
        else{
            tmpReplacedByAString = replacedByAString;
        }
    }
    else{
        tmpReplacedByAString = replacedByAString;
    }
    
    k = 0;
    i = 0;
    
    if (tmpReplacementString[i] == '^') {
        if (tmpReplacementString[i+1] == '\0') {
            replaceMod = 1;
        }
    }
    else{
        if (tmpReplacementString[i] == '$') {
            if (tmpReplacementString[i+1] == '\0') {
                replaceMod = 2;
            }
        }
    }
    
    switch (replaceMod) {
            
        case 0:{
            if (tmpReplacementString[i] == '\0') {
                while ((tempChar = getchar()) != EOF) {
                    while (tmpReplacedByAString[i] != '\0') {
                        putchar(tmpReplacedByAString[i]);
                        i++;
                    }
                    i = 0;
                }
                putchar(tempChar);
                return 0;
            }
            
            while ((tempChar = getchar()) != EOF) {
                if (tempChar == tmpReplacementString[i]) {
                    opaEstChe = 1;
                    i++;
                }
                else{
                    putchar(tempChar);
                }
                
                if (opaEstChe) {
                    while (1) {
                        if (!k) {
                            buffer = (char*)malloc(k + 1);
                        }
                        else{
                            buffer = (char*)realloc(buffer, k + 1);
                        }
                        buffer[k] = tempChar;
                        k++;
                        if (tmpReplacementString[i] != '\0') {
                            tempChar = getchar();
                            
                            if (tempChar == EOF) {
                                i = 0;
                                opaEstChe = 0;
                                break;
                            }
                            
                            if (tempChar == tmpReplacementString[i]) {
                                i++;
                            }
                            else{
                                i = 0;
                                opaEstChe = 0;
                                break;
                            }
                        }
                        else{
                            i = k = 0;
                            free(buffer);
                            break;
                        }
                    }
                    
                    if (opaEstChe) {
                        while (tmpReplacedByAString[i] != '\0') {
                            putchar(tmpReplacedByAString[i]);
                            i++;
                        }
                        i = 0;
                        opaEstChe = 0;
                    }
                    else{
                        for (i = 0; i < k; i++) {
                            putchar(buffer[k]);
                        }
                        free(buffer);
                        i = 0;
                        k = 0;
                        if (tempChar == EOF) {
                            putchar(tempChar);
                            break;
                        }
                        else{
                            putchar(tempChar);
                        }
                    }
                }
            }
            break;
        }
            
        case 1:{
            if (tmpReplacedByAString[i] == '\0') {
                while ((tempChar = getchar()) != EOF) {
                    putchar(tempChar);
                }
                putchar(tempChar);
                return 0;
            }
            
            while (1) {
                if ((tempChar = getchar()) != EOF){
                    while (1) {
                        while (tmpReplacedByAString[i] != '\0') {
                            putchar(tmpReplacedByAString[i]);
                            i++;
                        }
                        if ((tempChar != '\n') && (tempChar != EOF)) {
                            putchar(tempChar);
                            tempChar = getchar();
                        }
                        else{
                            putchar(tempChar);
                            i = 0;
                            if (tempChar == EOF) {
                                return 0;
                            }
                            else{
                                break;
                            }
                        }
                    }
                }
                else{
                    putchar(tempChar);
                    return 0;
                }
            }
            break;
        }
            
        case 2:{
            if (tmpReplacedByAString[i] == '\0') {
                while ((tempChar = getchar()) != EOF) {
                    putchar(tempChar);
                }
                putchar(tempChar);
                return 0;
            }
            
            while (1) {
                if ((tempChar = getchar()) != EOF){
                    while (1) {
                        while ((tempChar != '\n') && (tempChar != EOF)) {
                            putchar(tempChar);
                            tempChar = getchar();
                        }
                        while (tmpReplacedByAString[i] != '\0') {
                            putchar(tmpReplacedByAString[i]);
                            i++;
                        }
                        putchar(tempChar);
                        i = 0;
                        if (tempChar == EOF) {
                            return 0;
                        }
                        else{
                            break;
                        }
                    }
                }
                else{
                    putchar(tempChar);
                    return 0;
                }
            }
            break;
        }
            
        default:
            break;
    }
    
    return 0;
}

int mcat(char *str)
{
    FILE *file;
    char c;
    if (str == NULL)
        return 0;
    
    file = fopen(str, "r");
    
    c = fgetc(file);
    while (c != EOF)
    {
        putchar(c);
        c = fgetc(file);
    }
    fclose(file);
    return 0;
}

int mgrep(char *s, int v_flag)
{
    int vflag = 0, letter = 0, i = 0;
    char *tmpstr = NULL, *str = NULL;
    FILE *fd = stdin;
    
    str = s;
    
    while (1)
    {
        tmpstr = NULL;
        
        i = 0;
        
        while (1)
        {
            letter = getc(fd);
            
            tmpstr = (char *) realloc(tmpstr, (++i)*sizeof(char));
            
            if ((letter == EOF) || (letter == '\n'))
            {
                tmpstr[i-1] = '\0';
                break;
            }
            else
                tmpstr[i-1] = letter;
        }
        
        if (((vflag == 0) && (strstr(tmpstr, str) != NULL)) || ((vflag == 1) && (strstr(tmpstr, str) == NULL)))
            printf("%s\n",tmpstr);
        
        if (tmpstr != NULL)
            free(tmpstr);
        
        if (letter == EOF)
            break;
    }
    
    str = NULL;
    
    return 0;
    
}
