#include <stdio.h>
#include <ncurses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*This program is the 'launcher' for each of the levels, it generates a settings file and then runs the game.c file*/

/*TODO: Calculate actual size of readme file
        Add scroll support to readme view*/

/*GLOBAL VARS*/
char *option[] = {"Start Game", "Difficulty", "Seed", "How-To", "Exit", (char *)NULL,};
char *value[] = {"", "140", "1000", ""};
int selection;

int numLevels;

int write_settings()
{
    FILE *fp;

    //couldn't work out a way to (easily...) fully overwrite an existing file so we'll delete it just in case
    remove("settings.txt");
    
    //calc number of options
    int numOptions = sizeof(option) / sizeof(option[0]);

    //write corresponding options and values to file
    fp = fopen("settings.txt", "w");
    for(int i = 0; i < numOptions - 2; i++)
    {
        fprintf(fp, "%s : %s\n", option[i], value[i]);
    }
    fclose(fp);
    return 0;
}

int show_readme()
{
    //allocate enough space for the readme file
    char *readme = malloc(1000000);

    //load file like in write_settings, just with read perms
    FILE *fp;

    fp = fopen("readme.txt", "r");
    char c;
    int i=0;
    //go through char by char and add to readme array
    while ((c = fgetc(fp)) != EOF)
    {
        readme[i] = c;
        i++;
    }
    fclose(fp);

    //TODO: allow the user to scroll the text
    scrollok(stdscr, TRUE);
    idlok(stdscr, TRUE);
    printw("README:\n\n\n");
    printw(readme);
    refresh();
    for(;;)
    {
        int input;
        if((input = getch()) == ERR)
        {
            
        }
        else
        {
            switch(input)
            {
                case 10: 
                    //TODO: Go back to main menu
                    break;
            }
        }
    }
}

int selection_handler(int selection, MENU* mainMenu, ITEM** menuItems)
{
    switch(selection)
    {
        //for the level selections, only edit the level value then write to txt
        case 0:
            value[0] = "1";
            write_settings();
            break;
        
        case 1:
            value[0] = "2";
            write_settings();
            break;

        case 3:
            show_readme();
            break;
        
        case 4:
            //make sure we deallocate memory before exiting
            for(int i = 0; i < numLevels; ++i)
            {
                free_item(menuItems[i]);
            }  
	        free_menu(mainMenu);
	        endwin();
            exit(0);
            break;
            
    }
    return 0;
}

int main()
{
    ITEM** menuItems;
    MENU* mainMenu;

    //initialise screen and capture keyboard input
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);

    int numOptions = sizeof(option) / sizeof(option[0]);
    menuItems = (ITEM **)calloc(numOptions + 1, sizeof(ITEM *));

    for(int i = 0; i < numOptions; ++i)
    {
        menuItems[i] = new_item(option[i], value[i]);
    }  
	menuItems[numOptions] = (ITEM *)NULL;
    //populate menu data then print
    mainMenu = new_menu((ITEM **)menuItems);
	post_menu(mainMenu);
    refresh();
    //capture keyboard input and assign to choice
    int choice;
    while((choice = getch()) != ERR)
	{   
        switch(choice)
	    {	
            case KEY_DOWN:
		        menu_driver(mainMenu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(mainMenu, REQ_UP_ITEM);
				break;
            case 10:
                //10 represents enter, so get current item and pass to selection handler
                selection = item_index(current_item(mainMenu));
                selection_handler(selection, mainMenu, menuItems);
		}
	}	

    //something's gone wrong to get here, so deallocate memory before exiting...
    for(int i = 0; i < numLevels; ++i)
    {
        free_item(menuItems[i]);
    }  
	free_menu(mainMenu);
	endwin();
    return 0;
}