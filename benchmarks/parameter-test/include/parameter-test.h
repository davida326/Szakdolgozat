#ifndef PARAMETER_TEST_H
#define PARAMETER_TEST_H

#include <ncurses.h>        //ncurses
#include <sys/resource.h>   //a prioritás beállításához szükséges
#include <unistd.h>         //a prioritáshoz meg kell mondjam a szülő process pidjét, amihez le kell kérjem azt és ebben van a getpid()
#include <stdio.h>          //kiíratásokhoz/fájlkezeléshez
#include <stdlib.h>         //shell parancs kiadásához
#include <string.h>         //benchmark eredmények változóba mozgatásához strcpy
#include <signal.h>         //SIGINT(^C), SIGTERM signalok lekezeléséhez
#include <fcntl.h>          //open() -> /dev/null megnyitásához
#include <time.h>    
#include <math.h>

#define SAMPLECOUNT_DEFAULT 5
#define WIDTH 30
#define HEIGHT 15
#define DELETE 0
#define BUILD 1
#define LATENCY 0
#define MIN_GRAN 1
#define WAKEUP_GRAN 2
#define PRIORITY 3
#define VM_SWAPPINESS 4

void  SIGhandler(int);
WINDOW *centerWindow();
WINDOW *createWindow(int,int,int,int);
void destroyWindow(WINDOW *);
int menuPosition();
void printMenu(WINDOW *,int);
int selectedTest(int *,int *);
void startTest(int,int,int);
void commandBuilder(int testNum,int sampleCount,char *);
void preparations();
void resultPathBuilder(int testNum,char *path);
void resetParameters();
void setFileName(char *fileName);
void printMenuOnRun(WINDOW *localWin,char (*resultsArr)[255],int size,int testCount,int maxTest,int selected);
void pushNextResult(char (*resultsArr)[255],int size,int *i,char *result);

#endif