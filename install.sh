#!/bin/bash

gcc main.c -o main -lncurses -lmenu
gcc game.c -o game -lncurses -lmenu -lpthread

echo Install Complete - Run ./launch.sh to start the game.
echo Please note - for full functionality, please ensure you\'re running the gnome terminal
echo If you\'re not using the gnome terminal, please open the ./main file, select start game, then run the ./game file