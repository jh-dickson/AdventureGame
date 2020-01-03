#include <stdio.h>
#include <ncurses.h>
#include <menu.h>
#include <stdlib.h> 
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h> //<- technically speaking ncurses isn't really threadsafe but it works haha... see: https://stackoverflow.com/questions/29910562/why-has-no-one-written-a-threadsafe-branch-of-the-ncurses-library

/*GLOBAL VARIABLES*/
char *option[]; //<-used when loading the settings file, as with value
char *value[];
char *inventoryItems[4] = {"Health", "Hunger", "Swords", "Stars"}; //<- lets stats keep track of different character traits
int inventoryValues[4] = {5, 0, 0, 0};
int recentInput; 
char charUnderCursor;
int movements;

/*pthread definitions - this simplifies the locks and unlocks, see above stackoverflow link*/
pthread_mutex_t MUTEX;
#define LOCK pthread_mutex_lock(&MUTEX)
#define UNLOCK pthread_mutex_unlock(&MUTEX)

//simplify window creation process
WINDOW *create_newwin(int height, int width, int starty, int startx) {
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);
    wrefresh(local_win);
    return local_win;
}

int comp_move()
{

}

int can_move(int diffX, int diffY, char worldItems[4])
{
    //diffX is the change in X, same idea with diffY
    //worldItems array is the set of items 0:up 1:down 2:left 3:right from the moving object
    int i;
    switch (diffX)
    {
        case 1:
            i = 3;
            break;
        case -1:
            i = 2;
            break;
        default:
            break;
    }
    switch (diffY)
    {
        case 1:
            i = 1;
            break;
        case -1:
            i = 0;
            break;
        default:
            break;
    }

    switch (worldItems[i])
    {
        case 'T':
            return FALSE;
            break;
        case 'W':
            //one liner for nanosleep - water should slow you down a bit
            nanosleep((const struct timespec[]){{0, 500000000L}}, NULL);
            return TRUE;
            break;
        case ' ':
            return TRUE;
            break;
        case 'B':
            return TRUE;
            break;
        default:
            return TRUE;
            break;
    }



}

int open_settings()
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
    int j;
    while(fgets(line, 100, fp) != NULL)
    {
        //parse the line - first word is option, second is value
        int i;
        char tmpOpt[100];
        char tmpVal[100];
        while(line[i] != ' ')
        {
            tmpOpt[i] = line[i];
            i++;
        }
        strcpy(option[j], tmpOpt);
        i += 1;
        int k;
        while(line[i] != '\n')
        {
            tmpVal[k] = line[i];
            k++;
            i++;
        }
        strcpy(value[j], tmpVal);
        j++;

    }
    fclose(fp);
}

void *stats_handler(void *p)
{
    WINDOW *statsWindow;
    LOCK;
    statsWindow = create_newwin(LINES*0.2, COLS, LINES*0.8, 0);
    UNLOCK;

    for(;;)
    {
        if(recentInput == 0)
        {
            //run the hunger stats: every 5 moves, the player hunger increases
            if (movements % 5 == 0)
            {
                inventoryValues[1]+=1;
                LOCK;
                movements = 0;
                UNLOCK;
            }
            

        }
        else
        {    
            switch (recentInput)
            {
            case KEY_BACKSPACE:
                if(charUnderCursor == 'B')
                {
                    inventoryValues[1]++;
                    mvwprintw(statsWindow, 1, 1,"Picked up sword");
                }
                else
                {
                    mvwprintw(statsWindow, 1, 1,"No item here!");
                }
                LOCK;
                recentInput = 0;
                charUnderCursor = '\0';
                UNLOCK;
                break;
            case 10:
                mvwprintw(statsWindow, 2, 1,"No enemies here!");

                LOCK;
                recentInput = 0;
                UNLOCK;
                break;
            }
            //get inventory and print
            for (int i = 0; i < 4; i++)
            {
                mvwprintw(statsWindow, (i+1), (COLS*0.5), "%s: %d", inventoryItems[i], inventoryValues[i]);
            }
            wrefresh(statsWindow);
        }
    }
}

void *game_handler(void *p)
{
    WINDOW *gameWindow;
    LOCK;
    gameWindow = create_newwin(LINES*0.8, COLS, 0, 0);
    UNLOCK;
    char map[LINES][COLS];

    //generate world and print to game window
    //W - water (11%), [SPACE] - grass (87%), T - Tree (2%)
    const char *charToAdd;
    int seed = 100;
    //initialise colour settings for printing - assume the terminal has colour support... it probably does...
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_GREEN); //<- grass (green background and yellow characters)
    init_pair(2, COLOR_BLUE, COLOR_CYAN); //<- water
    init_pair(3, COLOR_BLACK, COLOR_GREEN); //<- trees
    init_pair(4, COLOR_WHITE, COLOR_RED); //<- boxes

    for (int i = 1; i < (LINES*0.8)-1; i++)
    {
        for (int j = 1; j < COLS-1; j++)
        {
            srand(seed);
            int nextCharType = rand() % 1000;
            if (nextCharType <= 86)
            {
                charToAdd = "T";
                wattron(gameWindow, COLOR_PAIR(3)); //<- this has to be done inside the if statement as the charTypes have different colours
                mvwprintw(gameWindow, i, j, charToAdd);
                wattroff(gameWindow, COLOR_PAIR(3));
            }
            else if (nextCharType > 86 && nextCharType <= 120)
            {
                charToAdd = "W";
                wattron(gameWindow, COLOR_PAIR(2)); //<- this has to be done inside the if statement as the charTypes have different colours
                mvwprintw(gameWindow, i, j, charToAdd);
                wattroff(gameWindow, COLOR_PAIR(2));
            }
            else if (nextCharType > 120 && nextCharType <= 121)
            {
                charToAdd = "B";
                wattron(gameWindow, COLOR_PAIR(4)); //<- this has to be done inside the if statement as the charTypes have different colours
                mvwprintw(gameWindow, i, j, charToAdd);
                wattroff(gameWindow, COLOR_PAIR(4));
            }
            else
            {
                charToAdd = " ";
                wattron(gameWindow, COLOR_PAIR(1)); //<- this has to be done inside the if statement as the charTypes have different colours
                mvwprintw(gameWindow, i, j, charToAdd);
                wattroff(gameWindow, COLOR_PAIR(1));
            }
            seed++;
        }
    }

    //set cursor to the centre of the gameWindow
    int cursorX = COLS/2;
    int cursorY = LINES*0.4;
    wmove(gameWindow, cursorY, cursorX);
    wrefresh(gameWindow);
    
    //take user input and handle accordingly
    int input;
    nodelay(gameWindow, TRUE);
    keypad(gameWindow, true);
    for(;;)
    {
        if((input = wgetch(gameWindow)) == ERR)
        {
            //TODO: yield thread? - not really useful since we're only a few threads here so probably rescheduled immediately
        }
        else
        {
            //look around the character so we can tell whether to move or not.
            char worldItems[4];
            worldItems[0] = mvwinch(gameWindow, cursorY - 1, cursorX);
            worldItems[1] = mvwinch(gameWindow, cursorY + 1, cursorX);
            worldItems[2] = mvwinch(gameWindow, cursorY, cursorX - 1);
            worldItems[3] = mvwinch(gameWindow, cursorY, cursorX + 1);
            wmove(gameWindow, cursorY, cursorX);
            
            //this also handles input for the stats screen using global variable recentInput
            switch(input)
            {
                case KEY_UP: 
                    //character movements are a weird routine which should really be in a function but i couldn't work that out in time
                    if (can_move(0,-1, worldItems) == TRUE)
                    {
                        cursorY--;
                        wmove(gameWindow, cursorY, cursorX);
                        //let stats know the player moved so we can increase hunger
                        LOCK;
                        movements++;
                        UNLOCK;
                    }
                    break;
                case KEY_DOWN:
                    if (can_move(0, 1, worldItems) == TRUE)
                    {
                        cursorY++;
                        wmove(gameWindow, cursorY, cursorX);
                        //let stats know the player moved so we can increase hunger
                        LOCK;
                        movements++;
                        UNLOCK;
                    }
                    break;
                case KEY_LEFT:
                    if (can_move(-1, 0, worldItems) == TRUE)
                    {
                        cursorX--;
                        wmove(gameWindow, cursorY, cursorX);
                        //let stats know the player moved so we can increase hunger
                        LOCK;
                        movements++;
                        UNLOCK;
                    }
                    break;
                case KEY_RIGHT:
                    if (can_move(1, 0, worldItems) == TRUE)
                    {
                        cursorX++;
                        wmove(gameWindow, cursorY, cursorX);
                        //let stats know the player moved so we can increase hunger
                        LOCK;
                        movements++;
                        UNLOCK;
                    }
                    break;
                case KEY_BACKSPACE:
                    //offload to stats handler
                    LOCK;
                    recentInput = KEY_BACKSPACE;
                    charUnderCursor = winch(gameWindow);
                    UNLOCK;
                    break;
                case 10:
                    //offload to stats handler
                    LOCK;
                    recentInput = 10;
                    charUnderCursor = winch(gameWindow);
                    UNLOCK;
                    break;
                
            }
            LOCK;
            wrefresh(gameWindow);
            UNLOCK;
        
        }
        

    }
}

int main()
{
    //open_settings();
    //printf("%s, %s", option[0], value[0]);

    //initialise screen
    initscr();
    cbreak();
    noecho();


    //create window threads
    pthread_t statsThread;
    pthread_t gameThread;
    pthread_create(&statsThread, NULL, stats_handler, NULL);
    pthread_create(&gameThread, NULL, game_handler, NULL);

    //wait for thread completion - this probably won't happen given the infinite loops...
    pthread_join(statsThread, NULL);
    pthread_join(gameThread, NULL);

    //cleanup windows
    endwin();
    return(0);
}