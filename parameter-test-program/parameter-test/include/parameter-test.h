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

void preparations(char *testName);
void selectedTest(int *sampleCount,int *intervalSplit,char *testName);
void getStringWithMenu(char *requestUpper,char *requestLower,char *str,int x,int y);
int getIntegerWithMenu(char *request);
WINDOW *centerWindow();
WINDOW *createWindow(int height,int width,int starty,int startx);
void destroyWindow(WINDOW *localWin);
void  SIGhandler(int sig);
void resetParameters();
void startTest(int intervalSplit,int sampleCount,char *testName);
void initializeArr(char (*resultsArr)[255],int size);
void pushNextResult(char (*resultsArr)[255],int size,int *i,char *result);
void printMenuOnRun(WINDOW *localWin,char (*resultsArr)[255],int size,int testCount,int maxTest,int selected,char *testName);
void setFileName(char *fileName);
void benchmarkCleanUp(char *testName);
void benchmarkStart(char *command);
void resultPathBuilder(char *testName,char *path);
void setParameter(int parameter,int value);
void commandBuilder(char *testName,int sampleCount,char *command);
int getParameter(int parameter,int n,int iteration);

#endif
