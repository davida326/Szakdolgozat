#include "parameter-test.h"
#include "xmlparser.h"

typedef struct parameters{       /* ütemező hangolási és swap memória változók, prioritási értékek */
    int latency;                 /* ezeket fogom tesztelni hogy, milyen mértékben befolyásolják    */
    int min_gran;                /* a teszt eredményeket                                           */
    int wakeup_gran;
    int priority;
    int vm_swappiness;
}parameters;

char *testList[] = {           /* maguk a tesztek nevei  */
    "sample-program",          /* és egy exit string,    */
    "ebizzy",                  /* az ncurses menühöz     */
    "fs-mark",
    "stream",
    "ctx-clock",
    "glmark2",
    "exit"
};

char *testListResult[] = {        /* phoronix-test-suite program      */   
    "sampleprogramres",           /* segítségével lementem a tesztek  */
    "ebizzyres",                  /* eredményeit, ezekkel a nevekkel  */
    "fsmarkres",
    "streamres",
    "ctxclockres",
    "glmark2res",
};

int testListSize = sizeof(testList)/sizeof(char *);
void preparations(){                                          /* ez a paraméter azt szabályozza hogy, */
    system("sysctl kernel.sched_tunable_scaling=0");          /* az ütemező is módosíthatja a         */
                                                              /* sched_latency_ns változó értékét     */
                                                              /* ( ezt én nem szeretném )             */
                                                              /*--------------------------------------*/
    writeConfig("/etc/phoronix-test-suite.xml");              /* batch-benchmark configurációs        */
}                                                             /* beállításait végzi el                */

int selectedTest(int *sampleCount,int *intervalSplit){
    initscr();
    raw();
    int x,y,ch,select = 0;
    getmaxyx(stdscr,y,x);
    printw("press q to exit...");
    refresh();
    noecho();
    WINDOW *menu = createWindow(HEIGHT,WIDTH,(y-HEIGHT)/2,(x-WIDTH)/2);
    printMenu(menu,select);

    while(1){
        ch = wgetch(menu);
        switch(ch){
            case KEY_UP: 
                if(select == 0) select = (testListSize-1);
                else --select;
            break;
            case KEY_DOWN:
                if(select == (testListSize-1)) select = 0;
                else ++select;
            break;
            case 113:
                endwin();
                exit(0);
            case 10:
                if(select == (testListSize-1)){
                    endwin();
                    exit(0);
                }
                else{
                    destroyWindow(menu);
                    *sampleCount = getIntegerWithMenu("sample count:",x,y);
                    *intervalSplit = getIntegerWithMenu("parameter split:",x,y);
                    return select;
                } 
        }
        wrefresh(menu);
        printMenu(menu,select);
    }
}

int getIntegerWithMenu(char *request,int x,int y){
    WINDOW *menu = createWindow(HEIGHT,WIDTH,(y-HEIGHT)/2,(x-WIDTH)/2);
    mvwprintw(menu,(HEIGHT/3),(WIDTH-strlen(request))/2,request);
    echo();
    int dummy=0;
    mvwscanw(menu,(HEIGHT/3),((WIDTH+strlen(request))/2),"%d",&dummy);
    mvwprintw(menu,2,0,"%d",dummy);
    wrefresh(menu);
    destroyWindow(menu);
    return (dummy) ? dummy : SAMPLECOUNT_DEFAULT;  /* alapérték beállítása, amennyiben nem adunk meg értéket */
}

int menuPosition(){
    int max = strlen(testList[0]);
    for(int i = 0;i<testListSize;i++)
        if(max < strlen(testList[i]))
            max = strlen(testList[i]);
    return max;
}

WINDOW *createWindow(int height,int width,int starty,int startx){
    WINDOW *localWin = newwin(height, width,starty,startx);
    keypad(localWin,TRUE);
    box(localWin,0,0);
    wrefresh(localWin);
    return localWin;
}

void destroyWindow(WINDOW *localWin){
    wborder(localWin,' ',' ',' ',' ',' ',' ',' ',' ');
    wrefresh(localWin);
    delwin(localWin);
}

void printMenu(WINDOW *menu,int selected){
    int x,y,wordLength = menuPosition();
    getmaxyx(menu,y,x);
    char *mesg="Please choose a benchmark";
    mvwprintw(menu,1,(x-strlen(mesg))/2,mesg);
    for(int i=0;i<testListSize;i++){
        if(selected == i)
            wattrset(menu,A_STANDOUT);
        else
            wattrset(menu,A_NORMAL); 
        mvwprintw(menu,((y-testListSize)/2)+i,(x-wordLength)/2,testList[i]);
        wrefresh(menu);
    }
}
void  SIGhandler(int sig)
{
    switch (sig) {
        case SIGINT:
            resetParameters();
            exit(0);
        break;
        case SIGTERM:
            resetParameters();
            exit(0);
        break; 
    }
}
void resetParameters(){
    system("sysctl kernel.sched_tunable_scaling=1");
    system("sysctl kernel.sched_latency_ns=12000000");
    system("sysctl kernel.sched_min_granularity_ns=1500000");
    system("sysctl kernel.sched_wakeup_granularity_ns=2000000");
}
void startTest(int testNum,int intervalSplit,int sampleCount){ 
    
    signal(SIGINT, SIGhandler);
    signal(SIGTERM, SIGhandler);
    char fileName[256];
    char command[200];
    char path[200];
    char result[100];
    int y,x,testCount = 1;
    parameters run;
    run.latency = 100000;
    run.min_gran = 100000;
    run.wakeup_gran = 0;
    run.priority = -20;
    run.vm_swappiness = 0;
   
    setFileName(fileName);
    FILE *fp = fopen(fileName,"w");

    fprintf(fp,"{\"testName\":\"%s\",\"measurements\":[",testList[testNum]);

    commandBuilder(testNum,sampleCount,command);
    resultPathBuilder(testNum,path);
    
    for(int i=0;i<intervalSplit;i++){
        setParameter(LATENCY,run.latency+getParameter(LATENCY,intervalSplit,i));
        for (int j = 0; j < intervalSplit; j++){
            setParameter(MIN_GRAN,run.min_gran+getParameter(MIN_GRAN,intervalSplit,j));
            for(int k = 0; k < intervalSplit; k++){
                setParameter(WAKEUP_GRAN,run.wakeup_gran+getParameter(WAKEUP_GRAN,intervalSplit,k));
                for(int l = 0; l < intervalSplit; l++){
                    setParameter(PRIORITY,run.priority+getParameter(PRIORITY,intervalSplit,l));
                    for(int m = 0; m < intervalSplit; m++,testCount++){
                        setParameter(VM_SWAPPINESS,run.vm_swappiness+getParameter(VM_SWAPPINESS,intervalSplit,m));
                        fprintf(fp,"{\"parameters\":{\"latency\":\"%d\",\"min_gran\":\"%d\",\"wakeup_gran\":\"%d\",\"priority\":\"%d\",\"vm.swappiness\":\"%d\"},",run.latency+getParameter(LATENCY,intervalSplit,i),run.min_gran+getParameter(MIN_GRAN,intervalSplit,j),run.wakeup_gran+getParameter(WAKEUP_GRAN,intervalSplit,k),run.priority+getParameter(PRIORITY,intervalSplit,l),run.vm_swappiness+getParameter(VM_SWAPPINESS,intervalSplit,m));
                        benchmarkStart(command,testCount);
                        getValueFromFile(path,result);
                        benchmarkCleanUp(testNum);
                        fprintf(fp,"\"result\":%s}%s",result,(!(i == (intervalSplit-1) && j == (intervalSplit-1) && k == (intervalSplit-1) && l == (intervalSplit-1) && m == (intervalSplit-1)) ? ", " : "]}"));
                    }
                }
            }
        }
    }
    fclose(fp);
    resetParameters(); 
    /* Remélhetőleg itt már elkészült a .json fájl és visszaállítom a paramétereket. */
}
void setFileName(char *fileName){
    time_t rawtime = time(NULL);
    struct tm *ptm = localtime(&rawtime);
    strftime(fileName, 256, "./logs/result%Y%m%d%H%M%S.json", ptm);
}
void benchmarkCleanUp(int testNum){
    char command[100];
    sprintf(command,"rm -rf /var/lib/phoronix-test-suite/test-results/%s",testListResult[testNum]);
    system(command);
};
void benchmarkStart(char *command,int testCount){
    printf("\nTest number: %d\n",testCount);
    system(command);    
}
void resultPathBuilder(int testNum,char *path){
    sprintf(path,"/var/lib/phoronix-test-suite/test-results/%s/composite.xml",testListResult[testNum]);
}

void setParameter(int parameter,int value){
    char *command;
    char *execute[255];
    switch(parameter){
        case LATENCY:
            command = "sysctl kernel.sched_latency_ns";
            break;
        case MIN_GRAN:
            command = "sysctl kernel.sched_min_granularity_ns";
            break;
        case WAKEUP_GRAN:
            command = "sysctl kernel.sched_wakeup_granularity_ns";
            break;
        case PRIORITY:
            setpriority(PRIO_PROCESS,getpid(),value);
            return;
            break;
        case VM_SWAPPINESS:
            command = "sysctl vm.swappiness";
            break;
    }
    sprintf(execute,"%s=%d",command,value);
    system(execute);
}

void commandBuilder(int testNum,int sampleCount,char *command){
    char *preset_option;
    switch(testNum){
        case 2:
            preset_option = "PRESET_OPTIONS=\"fs-mark.run-type=1000 Files, 1MB Size\"";
            break;
        case 3:
            preset_option = "PRESET_OPTIONS=\"stream.run-type=Copy\"";
            break;
        case 5:
            preset_option = "PRESET_OPTIONS=\"glmark2.run-type=800 x 600\"";
            break;
        default:
            preset_option = "";
    }
    sprintf(command,"TEST_RESULTS_DESCRIPTION=myDescription TEST_RESULTS_IDENTIFIER=myIdentity TEST_RESULTS_NAME=%s FORCE_TIMES_TO_RUN=%d %s phoronix-test-suite batch-benchmark %s",testListResult[testNum],sampleCount,preset_option,testList[testNum]);
}

int getParameter(int parameter,int n,int iteration){
    int latencyInterval = 999900000;
    int min_granInterval = 999900000;
    int wakeup_granInterval = 1000000000;
    int priorityInterval = 39;
    int vm_swappinessInterval = 100;
    switch(parameter){
        case LATENCY:
            return (latencyInterval/(n-1))*iteration;
        case MIN_GRAN:
            return (min_granInterval/(n-1))*iteration;
        case WAKEUP_GRAN:
            return (wakeup_granInterval/(n-1))*iteration;
        case PRIORITY:
            return (priorityInterval/(n-1))*iteration;
        case VM_SWAPPINESS:
            return (vm_swappinessInterval/(n-1))*iteration;
    }
}

