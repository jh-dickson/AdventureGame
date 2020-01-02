#include <stdio.h>
#include <ncurses.h>
#include <menu.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/*This program is the 'launcher' for each of the levels, it generates a settings file and then runs the game.c file*/

/*GLOBAL VARS*/
char *levels[] = {"Level 1", "Level 2", "Level 3", "Level 4", "Settings", "Exit", (char *)NULL,};
char *option[] = {"Level", "Difficulty", "Return to Main Menu", (char *)NULL,};
char *value[] = {"1", "5", ""};
int selection;
ITEM** levelItems;
MENU* mainMenu;
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

int show_settings_menu()
{
    ITEM** optionItems;
    MENU* optionMenu;

    //clear up main menu
    for(int i = 0; i < numLevels; ++i)
    {
        free_item(levelItems[i]);
    }  
	free_menu(mainMenu);
    endwin(); //<-this helps prevent the wrong menu items being displayed
    refresh();
    
    //restart window
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);

    //copied and pasted from main()
    //clear screen and display new menu
    //look at the options and see how many there are, then assign a menu item to each
    int numOptions = sizeof(option) / sizeof(option[0]);
    optionItems = (ITEM **)calloc(numOptions + 1, sizeof(ITEM *));
    for(int i = 1; i < numOptions; ++i)
    {
        optionItems[i] = new_item(option[i], value[i]);
    }  
	optionItems[numOptions] = (ITEM *)NULL;
    //populate menu data then print
    optionMenu = new_menu((ITEM **)optionItems);
	post_menu(optionMenu);
    refresh();

}

int selection_handler(int selection)
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

        case 2:
            value[0] = "3";
            write_settings();
            break;
        
        case 3:
            value[0] = "4";
            write_settings();
            break;
        
        case 4:
            show_settings_menu();
            break;

        case 5:
            //make sure we deallocate memory before exiting
            for(int i = 0; i < numLevels; ++i)
            {
                free_item(levelItems[i]);
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
    //initialise screen and capture keyboard input
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);

    //look at the levels and see how many options there are, then assign a menu item to each level
    numLevels = sizeof(levels) / sizeof(levels[0]);
    levelItems = (ITEM **)calloc(numLevels + 1, sizeof(ITEM *));
    for(int i = 0; i < numLevels; ++i)
    {
        levelItems[i] = new_item(levels[i], levels[i]);
    }  
	levelItems[numLevels] = (ITEM *)NULL;
    
    //populate menu data then print
    mainMenu = new_menu((ITEM **)levelItems);
    menu_opts_off(mainMenu, O_SHOWDESC); //<- workaround for duplicated columns - disables item descriptions
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
                selection_handler(selection);
		}
	}	

    //something's gone wrong to get here, so deallocate memory before exiting...
    for(int i = 0; i < numLevels; ++i)
    {
        free_item(levelItems[i]);
    }  
	free_menu(mainMenu);
	endwin();
    return 0;
}