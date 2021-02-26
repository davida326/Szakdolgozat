#include "parameter-test.h"
#include "xmlparser.h"

typedef struct parameters{       /* ütemező hangolási és swap memória változók, prioritási értékek */
    int latency;                 /* ezeket fogom tesztelni hogy, milyen mértékben befolyásolják    */
    int min_gran;                /* a teszt eredményeket                                           */
    int wakeup_gran;
    int priority;
    int vm_swappiness;
}parameters;

void printReverse(configList *head,int i,WINDOW *win) { 
    // Base case   
    if (head == NULL) 
       return; 
    i--;
    // print the list after head node 
    printReverse(head->next,i,win); 
    
    // After everything else is printed, print head 
    mvwprintw(win,i+2,1,"%d - %s",i, head->value); 
} 

void preparations(char *testName){                      
    // teszt telepítés
    system("sysctl kernel.sched_tunable_scaling=0 > /dev/null"); /* Ezt a paramétert 0-ra állítottam, */        
    char command[255],testVersion[255];                          /* annak érdekében hogy az ütemező   */
    printw("installing test...");                                /* ne módosítsa a paramétereket.     */
    refresh();
    sprintf(command,"phoronix-test-suite install %s > /dev/null",testName);
    system(command);                                          /* teszt telepítése */
    clear();
    refresh();

    //PHASE2 - teszt verziószám, config-xml fájlok keresése
    sprintf(command,"ls /var/lib/phoronix-test-suite/installed-tests/pts | grep %s > testVersion.txt",testName);
    system(command);
    FILE *version = fopen("testVersion.txt","r");
    fscanf(version,"%s",testVersion);                         
    fclose(version);
    remove("testVersion.txt");
    char location[255];
    sprintf(location,"/var/lib/phoronix-test-suite/test-profiles/pts/%s/test-definition.xml",testVersion);
    /* a test-defininition.xml fájlban találhatók meg a teszteknél kiválasztható konfigurációk listája */
    
    //config fájlok módosítása/teszt benchmark előkészítése
    int y,x;
    getmaxyx(stdscr,y,x);
    WINDOW *win = createWindow(HEIGHT,WIDTH*2,((y-HEIGHT)/2),(x-WIDTH*2)/2); // új ablak létrehozása 
    configList *nodeList = NULL;    // láncolt listában tárolom el a lehetséges beállítási kategóriákat
    refresh();
    xmlDoc *doc;                                            
    xmlNode *root;
    doc = xmlParseFile(location);       // fájl megnyitása
    root = xmlDocGetRootElement(doc);
    searchForNodes(root,doc,"Option",&nodeList);    // a fájlban megkeresi az összes "Option" elnevezési node-ot
    for(;nodeList!=NULL;nodeList=nodeList->next){   // az Option node-okon belül szükségem van két fontos elemre
        char displayName[80];                       // ezek a DisplayName és az identifier.
        char identifier[80];                        // A displayname a beállítás neve
                                                    // az identifier pedig az azonosítója
        int num=0,yline=1;
        configList *entryList = NULL;
        searchForNodeValue(nodeList->element,doc,"DisplayName",displayName); //kikeresem a két node értékét
        searchForNodeValue(nodeList->element,doc,"Identifier",identifier);
        int defaultEntry=0;
        searchForDefaultEntry(nodeList->element,"DefaultEntry",&defaultEntry); // megnézem hogy van-e alapértelmezett beállítás ezekre a
        char input[80];                                                        // konfigurációkra
        mvwprintw(win,yline,1,"%s",displayName);yline++;
        wmove(win,yline+1,0);
        
        if(!strcmp(identifier,"auto-resolution")){     //amennyiben az azonosító "auto-resolution", sajnos nem tudom megjeleníteni az opciókat hozzá
            mvwprintw(win,yline,1,"Enter the id of the resolution, you want.");yline++; //azonban az input ugyanúgy feldolgozható 
            mvwprintw(win,yline,1,"Resolutions can be different from one test, to another.");yline++;
            mvwprintw(win,yline,1,"(goes from 1 to N)");yline++;
            mvwprintw(win,yline,1,"You can see the resolutions available,");yline++;
            mvwprintw(win,yline,1,"at openbenchmarking.orgtest/pts/your_test");yline++;
            mvwprintw(win,yline,1,"Configuration(positive integer):");yline++;
        }
        else{
            searchForNodeValues(nodeList->element,doc,"Name",&entryList,&num); // Minden más beállításhoz, megtalálható az .xml fájlban
            printReverse(entryList,num+1,win);                                 // a lehetséges opciók. Ezt egy másik láncolt listában tárolom.
            yline=yline+num+1;
            mvwprintw(win,yline,1,"Write the configuration number please, from above");yline++;
            mvwprintw(win,yline,1,"Configuration(positive integer):");yline++;
        }
        
        wrefresh(win);
        wscanw(win,"%s",&input);  
        if(defaultEntry)                             // Abban az esetben ha van alapértelmezett beállítás, azt módosítom
            writeNode(nodeList->element,"DefaultEntry",input);
        else
            insertNode(nodeList->element,input);     // egyébként újat szúrok be
        freeList(entryList); // felszabadítja a láncolt lista által lefoglalt memóriát
    }
    xmlSaveFormatFile(location, doc, 0);           // elmenti a módosításokat és felülírja a fájlt
	xmlFreeDoc(doc);                                              
    wclear(win);
    wrefresh(win);
    destroyWindow(win);     
    freeList(nodeList);          // felszabadítja a láncolt lista által lefoglalt memóriát
    writeConfig("/etc/phoronix-test-suite.xml");              /* batch-benchmark configurációs        */
}                                                             /* beállításait végzi el                */

void selectedTest(int *sampleCount,int *intervalSplit,char *testName){
    int x,y,ch,select = 0;
    getmaxyx(stdscr,y,x);                  
    getStringWithMenu("test name:pts/","(without version number)",testName,x,y); //tesztnév beállítása
    do{
        *sampleCount = getIntegerWithMenu("sample count:"); 
    }while(*sampleCount ==1);
    if(*sampleCount==5)
        printw("default value has been set for sample count\n");
    do{
        *intervalSplit = getIntegerWithMenu("parameter split:");
    }while(*intervalSplit==1);
    if(*intervalSplit==5)
        printw("default value has been set for the parameter split value\n");
}
void getStringWithMenu(char *requestUpper,char *requestLower,char *str,int x,int y){
    echo();
    WINDOW *strMenu = createWindow(HEIGHT,WIDTH+20,((y-HEIGHT)/2),(x-WIDTH-20)/2);

    
    mvwprintw(strMenu,(HEIGHT/3)+1,1,requestLower);
    mvwprintw(strMenu,(HEIGHT/3),1,requestUpper);
    wscanw(strMenu,"%s",str);

    wborder(strMenu,' ',' ',' ',' ',' ',' ',' ',' '); 
    wclear(strMenu);
    wrefresh(strMenu);
    delwin(strMenu);
}
int getIntegerWithMenu(char *request){
    WINDOW *menu = centerWindow();
    mvwprintw(menu,(HEIGHT/3),(WIDTH-strlen(request))/2,request);
    echo();
    int dummy=0;
    mvwscanw(menu,(HEIGHT/3),((WIDTH+strlen(request))/2),"%d",&dummy);
    mvwprintw(menu,2,0,"%d",dummy);
    wclear(menu);
    wrefresh(menu);
    destroyWindow(menu);
    return (dummy) ? dummy : SAMPLECOUNT_DEFAULT;  /* alapérték beállítása, amennyiben nem adunk meg értéket */
}

WINDOW *centerWindow(){
    int x,y;
    getmaxyx(stdscr,y,x);
    WINDOW *menu = createWindow(HEIGHT,WIDTH,((y-HEIGHT)/2),(x-WIDTH)/2);
    return menu;
}

WINDOW *createWindow(int height,int width,int starty,int startx){  /* egyszerű ablakot létrehozok a méretekkel,     */
    WINDOW *localWin = newwin(height, width,starty,startx);        /* és hogy az hol helyezkedjen el:startx,starty  */
    keypad(localWin,TRUE);
    box(localWin,0,0);
    wrefresh(localWin);
    return localWin;
}

void destroyWindow(WINDOW *localWin){                       /* ha csak kitörölném, ott maradna pár + jel            */
    wborder(localWin,' ',' ',' ',' ',' ',' ',' ',' ');      /* amit nem szeretnék,ezért így leszedem róla a keretet */
    delwin(localWin);
}


void  SIGhandler(int sig){
    switch (sig) {
        case SIGINT:                    /* SIGINT, (^C) illetve SIGTERM                  */
            resetParameters();          /* signalok esetén visszaállítom a paramétereket */
            exit(0);                    /* és a program megszünteti magát                */
        break;
        case SIGTERM:
            resetParameters();
            exit(0);
        break; 
    }
}
void resetParameters(){
    system("sysctl kernel.sched_tunable_scaling=1 > /dev/null");
    system("sysctl kernel.sched_latency_ns=12000000 > /dev/null");              /* ütemező hangoló paraméterek alapbeállításai */
    system("sysctl kernel.sched_min_granularity_ns=1500000 > /dev/null");
    system("sysctl kernel.sched_wakeup_granularity_ns=2000000 > /dev/null");
}
void startTest(int intervalSplit,int sampleCount,char *testName){ 
    
    signal(SIGINT, SIGhandler);
    signal(SIGTERM, SIGhandler);
    char fileName[256];
    char command[1024];
    char path[1024];
    char result[80];
    int y,x,size = 5,testCount = 1,testIndex = 0;
    parameters run;
    run.latency = 100000;
    run.min_gran = 100000;
    run.wakeup_gran = 0;
    run.priority = -20;
    run.vm_swappiness = 0;
    char lastResults[size][255];
    initializeArr(lastResults,size); /* kitölti --- jelekkel az eredményhalmazt */
    refresh();
    WINDOW *menuOnRun = centerWindow();
    cbreak();
    setFileName(fileName);                          /* a névben szerepel egy dátum, */
    FILE *fp = fopen(fileName,"w");                 /* így több teszt is futtatható,*/
                                                    /* egymást követően             */
    fprintf(fp,"{\"testName\":\"%s\",\"measurements\":[",testName);
    commandBuilder(testName,sampleCount,command);    
    resultPathBuilder(testName,path);
    for(int i=0;i<intervalSplit;i++){
        setParameter(LATENCY,run.latency+getParameter(LATENCY,intervalSplit,i));
         for (int j = 0; j < intervalSplit; j++){
            setParameter(MIN_GRAN,run.min_gran+getParameter(MIN_GRAN,intervalSplit,j));
            for(int k = 0; k < intervalSplit; k++){
                setParameter(WAKEUP_GRAN,run.wakeup_gran+getParameter(WAKEUP_GRAN,intervalSplit,k));
                for(int l = 0; l < intervalSplit; l++){
                    setParameter(PRIORITY,run.priority+getParameter(PRIORITY,intervalSplit,l));
                    for(int m = 0; m < intervalSplit; m++,testCount++,testIndex++){
                        menuOnRun = centerWindow();
                        printMenuOnRun(menuOnRun,lastResults,size,testCount,(int)pow(intervalSplit,5),testIndex);
                        setParameter(VM_SWAPPINESS,run.vm_swappiness+getParameter(VM_SWAPPINESS,intervalSplit,m));
                        fprintf(fp,"{\"parameters\":{\"latency\":\"%d\",\"min_gran\":\"%d\",\"wakeup_gran\":\"%d\",\"priority\":\"%d\",\"vm.swappiness\":\"%d\"},",run.latency+getParameter(LATENCY,intervalSplit,i),run.min_gran+getParameter(MIN_GRAN,intervalSplit,j),run.wakeup_gran+getParameter(WAKEUP_GRAN,intervalSplit,k),run.priority+getParameter(PRIORITY,intervalSplit,l),run.vm_swappiness+getParameter(VM_SWAPPINESS,intervalSplit,m));
                        benchmarkStart(command);
                        getValueFromFile(path,"Value",result);
                        pushNextResult(lastResults,size,&testIndex,result);
                        benchmarkCleanUp(testName);
                        fprintf(fp,"\"result\":%s}%s",result,(!(i == (intervalSplit-1) && j == (intervalSplit-1) && k == (intervalSplit-1) && l == (intervalSplit-1) && m == (intervalSplit-1)) ? ", " : "]}"));
                        destroyWindow(menuOnRun);
                            /* a paraméterként megkapott fájlútvonalat megnyitja,       */
                            /* az előző függvény segítségével, eltárolja az eredményt   */
                        /* az eredményt továbbadja a kapott char *val-nak           */
                        
                         
                        
                        
                    
                    }
                }
            }
        }
    }
    fclose(fp);
    resetParameters(); 
    /* Remélhetőleg itt már elkészült a .json fájl és visszaállítom a paramétereket. */
}
void initializeArr(char (*resultsArr)[255],int size){
    for(int i=0;i<size;i++)
        strcpy(resultsArr[i],"---");
}
void pushNextResult(char (*resultsArr)[255],int size,int *i,char *result){
    if(*i == (size-1)){      
        strcpy(resultsArr[*i],result);
        *i = -1;
    }
    else{
        strcpy(resultsArr[*i],result);
    }
}
void printMenuOnRun(WINDOW *localWin,char (*resultsArr)[255],int size,int testCount,int maxTest,int selected){
    int x,y;
    char *welcome = "Last five tests results:";
    char *current = "Current test:";
    char buff[255];
    sprintf(buff,"%d/%d",maxTest,testCount);
    getmaxyx(localWin,y,x);
    mvwprintw(localWin,2,(x-strlen(welcome))/2,welcome);
    for(int i=1;i<(size+1);i++){
        if(selected == (i-1))
            wattrset(localWin,A_STANDOUT);          
        else
            wattrset(localWin,A_NORMAL); 
        mvwprintw(localWin,(y-1)/3+i,2,"%s",resultsArr[i-1]);
    }
    wattrset(localWin,A_NORMAL);
    mvwprintw(localWin,(y-3),(x-strlen(current))/2,"%s",current);
    mvwprintw(localWin,(y-2),(x-strlen(buff))/2,"%s",buff);
    wrefresh(localWin);
}

void setFileName(char *fileName){
    time_t rawtime = time(NULL);                        
    struct tm *ptm = localtime(&rawtime);
    strftime(fileName, 256, "./logs/result%Y%m%d%H%M%S.json", ptm);
}
                                                            /* a törlés azért fontos mivel,                 */
void benchmarkCleanUp(char *testName){                         /* ugyanazzal a névvel, azonosítóval, leírással */
    char command[100];                                      /* indítom a következő tesztet                  */
    sprintf(command,"rm -rf /var/lib/phoronix-test-suite/test-results/%sres",testName);
    system(command);
};
void benchmarkStart(char *command){       
    system(command);    
}
void resultPathBuilder(char *testName,char *path){
    sprintf(path,"/var/lib/phoronix-test-suite/test-results/%sres/composite.xml",testName);
}

void setParameter(int parameter,int value){
    char *command;
    char execute[255];                     /* beállítjuk a kapott paramétert,      */ 
    switch(parameter){                      /* a kapott értékre sysctl-segítségével */
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
    sprintf(execute,"%s=%d > /dev/null",command,value);
    system(execute);
}

void commandBuilder(char *testName,int sampleCount,char *command){
    sprintf(command,"TEST_RESULTS_DESCRIPTION=myDescription TEST_RESULTS_IDENTIFIER=myIdentity TEST_RESULTS_NAME=%sres FORCE_TIMES_TO_RUN=%d PRESET_OPTIONS='%s' phoronix-test-suite batch-benchmark %s > /dev/null ",testName,sampleCount,testName,testName);
}

int getParameter(int parameter,int n,int iteration){   /* Az startTest fgv-ben a paraméterek be vannak állítva a legkisebb értékre     */
    int latencyInterval = 999900000;                   /* itt a található mindegyiknek a legnagyobb értéke.                            */     
    int min_granInterval = 999900000;                  /* Ennek a függvénynek a visszatérési értékét, mindig hozzáadom                 */
    int wakeup_granInterval = 1000000000;              /* az épp aktuális paraméter értékéhez.                                         */
    int priorityInterval = 39;                         /* paraméterek: & parameter - melyik változóról van szó                         */
    int vm_swappinessInterval = 100;                   /*              & n         - hány részre kellett szétszedni az intervallumot   */
    switch(parameter){                                 /*              & interation- éppen melyik iterációnál járunk és ezzel szorzunk */ 
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

