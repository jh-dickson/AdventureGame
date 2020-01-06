#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int value[5];

int main()
{
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
        int i = 0;
        char tmpVal[2];
        while(line[i] != ' ')
        {
            //legacy from when I was parsing option names
            i++;
        }
        i += 3;
        int k = 0;
        while(line[i] != '\n')
        {
           tmpVal[k] = line[i];
            k++;
            i++;
        }
        
        value[j] = atoi(tmpVal);
        j++;
    }
    printf("value[0]: %d\n", value[0]);
    printf("value[1]: %d\n", value[1]);
    fclose(fp);
}