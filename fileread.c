#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *option[5];
char *value[5];

int main()
{
    //open the settings file and read each line
    FILE *fp;
    char line[100];
    fp = fopen("settings.txt", "r");
    if (fp == NULL)
    {
        //oops looks like the file hasn't been created yet... ask the user to open main.c
        printf("There was an error opening the settings file, please ensure that settings.txt exists\n");
        printf("Running the menu screen again should solve this issue.\n");
        exit(1);
    }

    //read settings line by line
    int j = 0;
    while(fgets(line, 100, fp) != NULL)
    {
        //parse the line - first word is option, second is value
        int i = 0, k = 0;;
        char tmpOpt[100];
        char tmpVal[100];
        while(line[i] != ' ')
        {
            tmpOpt[k] = line[i];
            i++;
            k++;
        }
        
        i += 1;
        k = 0;
        while(line[i] != '\n')
        {
           tmpVal[k] = line[i];
            k++;
            i++;
        }
        printf("TmpVal: %s\n", tmpVal);
        j++;

    }
    fclose(fp);
}