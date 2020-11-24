#include <sys/resource.h>   //a prioritás beállításához szükséges
#include <unistd.h>         //a prioritáshoz meg kell mondjam a szülő process pidjét, amihez le kell kérjem azt és ebben van a getpid()
#include <stdio.h>          //kiíratásokhoz/fájlkezeléshez
#include <stdlib.h>         //shell parancs kiadásához
#include <string.h>         //benchmark eredmények változóba mozgatásához strcpy
#include <signal.h>         //SIGINT(^C), SIGTERM signalok lekezeléséhez
#define TESTS 5
void SIGhandler(int);
int setLatency(int *);
int setMin_gran(int *);
int setWakeup(int *);
int setPrio(int *);
int setVMSwap(int *);
FILE *initBenchmark(char *,int *);
void piStart(int);
void piFinish();
void ebizzyStart(int);
void ebizzyFinish();
void streamStart(int);
void streamFinish();
void fs_markStart(int);
void fs_markFinish();
void qgears2Start(int);
void qgears2Finish();
void parameterTestStart(FILE *,int,int);
void parameterTestFinish(int );
void resetPar();
typedef struct Parameters{
    int latency;
    int min_gran;
    int wakeup_gran;
    int prio;
    int swap;
}parameters;

int main(int argc, char *argv[]){
    int samplecount = 5; //default samplecount
    int benchmark = -1;
    signal(SIGINT, SIGhandler);
    signal(SIGTERM, SIGhandler);
    FILE *fp;
    if(argc == 2) fp = initBenchmark(argv[1],&benchmark);
    else if(argc == 3){
        fp = initBenchmark(argv[1],&benchmark);
        samplecount = atoi(argv[2]);
    }
    else{
        printf("# ./parameter-test <testname> <samplecount>\nAvailable tests:\npi\nebizzy\nstream\nfs_mark\nqgears2\n");
        exit(0);
    }
    parameterTestStart(fp,benchmark,samplecount);
    parameterTestFinish(benchmark);
    return 0;
}

void parameterTestFinish(int benchmark){
    remove("tmpresults");
    void (*endBenchmark[TESTS])(int n);
    endBenchmark[0]=piFinish;
    endBenchmark[1]=ebizzyFinish;
    endBenchmark[2]=streamFinish;
    endBenchmark[3]=fs_markFinish;
    endBenchmark[4]=qgears2Finish;
    (*endBenchmark[benchmark])(benchmark);
}
void parameterTestStart(FILE *fp, int benchmark,int samplecount){
    system("sysctl kernel.sched_tunable_scaling=0");
    fprintf(fp,"{ \"measurements\": [");
    parameters run;
    parameters real;
    int testcount=0;
    char buff[1024];
    void (*startBenchmark[TESTS])(int n);
    startBenchmark[0]=piStart;
    startBenchmark[1]=ebizzyStart;
    startBenchmark[2]=streamStart;
    startBenchmark[3]=fs_markStart;
    startBenchmark[4]=qgears2Start;
    for(run.latency = 0;run.latency < 4;run.latency++){
        real.latency=setLatency(&run.latency);
        for(run.min_gran = 0;run.min_gran < 4;run.min_gran++){
            real.min_gran = setMin_gran(&run.min_gran);
            for(run.wakeup_gran = 0;run.wakeup_gran < 4;run.wakeup_gran++){
                real.wakeup_gran = setWakeup(&run.wakeup_gran);
                for(run.prio = 0; run.prio < 4; run.prio++){
                    real.prio = setPrio(&run.prio);
                    for(run.swap = 0; run.swap <4; run.swap++){
                    real.swap = setVMSwap(&run.swap);
                    testcount++;
                    fprintf(fp,"{ \"parameters\":");
                    fprintf(fp,"{\"latency\":\"%d\", \"min_gran\":\"%d\", \"wakeup_gran\":\"%d\",\"prio\":\"%d\",\"vm.swap\":\"%d\"},\"results\":[",real.latency,real.min_gran,real.wakeup_gran,real.prio,real.swap);
                    printf("testnumber: %d\nlatency:%d min_gran:%d wakeup_gran:%d priority:%d vm.swappiness:%d\n",testcount,run.latency,run.min_gran,run.wakeup_gran,run.prio,run.swap);
                    FILE *result;
                    (*startBenchmark[benchmark])(samplecount);
                    char *iresult[samplecount];
                    result = fopen("tmpresults","r");
                    int i=0;
                    while(fscanf(result,"%s",buff)!=EOF){
                        printf("%s\n",buff);
                        iresult[i] = malloc(sizeof(char));
                        strcpy(iresult[i],buff);
                        i++;
                    }
                    for(int i=0;i<samplecount;i++){
                        fprintf(fp,"%s",iresult[i]);
                        if(i<(samplecount-1)){
                            fprintf(fp,",");
                        }
                        free(iresult[i]);
                    }
                    fprintf(fp,"]}");
                    if(!((run.latency==3) && (run.min_gran==3) && (run.wakeup_gran==3) && (run.prio==3) && (run.swap==3))) fprintf(fp,", ");  
                    }
                }
            }
        }
    }
    fprintf(fp,"]}");
    fclose(fp);
}
int setLatency(int *value){
    switch (*value){
    case 0:system("sysctl kernel.sched_latency_ns=100000");
        return 100000;
        break;
    case 1:system("sysctl kernel.sched_latency_ns=333400000");
        return 333400000;
        break;
    case 2:system("sysctl kernel.sched_latency_ns=666700000");
        return 666700000;
        break;
    case 3:system("sysctl kernel.sched_latency_ns=1000000000");
        return 1000000000;
        break;
    default:printf("ERROR!(setlatency)\n");
        break;
    }
}
int setMin_gran(int *value){
    switch (*value){
    case 0:system("sysctl kernel.sched_min_granularity_ns=100000");
        return 100000;
        break;
    case 1:system("sysctl kernel.sched_min_granularity_ns=333400000");
        return 333400000;
        break;
    case 2:system("sysctl kernel.sched_min_granularity_ns=666666666");
        return 666666666;
        break;
    case 3:system("sysctl kernel.sched_min_granularity_ns=1000000000");
        return 1000000000;
        break;
    default:printf("ERROR!(setmin_gran)\n");
        break;
    }
}
int setWakeup(int *value){
    switch (*value){
    case 0:system("sysctl kernel.sched_wakeup_granularity_ns=0");
        return 0;
        break;
    case 1:system("sysctl kernel.sched_wakeup_granularity_ns=333333333");
        return 333333333;
        break;
    case 2:system("sysctl kernel.sched_wakeup_granularity_ns=666666666");
        return 666666666;
        break;
    case 3:system("sysctl kernel.sched_wakeup_granularity_ns=1000000000");
        return 1000000000;
        break;
    default:printf("ERROR!(setmin_gran)\n");
        break;
    }
}
int setPrio(int *value){
    switch (*value){
    case 0:setpriority(PRIO_PROCESS,getpid(),-20);
        return -20;
        break;
    case 1:setpriority(PRIO_PROCESS,getpid(),-6);
        return -6;
        break;
    case 2:setpriority(PRIO_PROCESS,getpid(),6);
        return 6;
        break;
    case 3:setpriority(PRIO_PROCESS,getpid(),19);
        return 19;
        break;
    default:printf("ERROR!(set nice value)\n");
        break;
    }
}
int setVMSwap(int *value){
    switch (*value){
    case 0:system("sysctl vm.swappiness=0");//Swapping off
        return 0;
        break;
    case 1:system("sysctl vm.swappiness=33");
        return 33;
        break;
    case 2:system("sysctl vm.swappiness=66");
        return 66;
        break;
    case 3:system("sysctl vm.swappiness=100");//Agressive swapping
        return 100;
        break;
    default:printf("ERROR!(set vm.swappiness value)\n");
        break;
    }
}
FILE *initBenchmark(char *name,int *b){
    FILE *fp;
    if(strcmp(name,"pi")==0)
    {
        system("make --directory=../pi/pi-benchmark/");
        *b = 0;
        fp = fopen("../pi/logs/ertekek.json","w+");
        return fp;
    } 
    else if(strcmp(name,"ebizzy")==0){
        system("make --directory=../ebizzy/ebizzy-0.3/");
        *b = 1;
        fp = fopen("../ebizzy/logs/ertekek.json","w+");
        return fp;
    }
    else if(strcmp(name,"stream")==0){
        system("make --directory=../stream/stream-1.3.1/");
        *b = 2;
        fp = fopen("../stream/logs/ertekek.json","w+");
        return fp;
    }
    else if(strcmp(name,"fs_mark")==0){
        system("make --directory=../fs_mark/fs_mark-3.3/");
        *b = 3;
        fp = fopen("../fs_mark/logs/ertekek.json","w+");
        return fp;
    }
    else if(strcmp(name,"qgears2")==0){
        system("make --directory=../qgears2/qgears2-1.0.1/");
        *b = 4;
        fp = fopen("../qgears2/logs/ertekek.json","w+");
    }   
    else printf("no\n");
}
void fs_markStart(int samplecount){
    FILE *comm;      
    fclose(fopen("tmpresults", "w"));                              
    for(int i=0;i<samplecount;i++){                                
        comm = popen(" ../fs_mark/fs_mark-3.3/fs_mark -d scratch -s 1048576 -n 1000 >> tmpresults ","r");
        fclose(comm);
    }
}
void fs_markFinish(){
    system("../fs_mark/fs_mark-3.3/remove.sh");
    resetPar();
}
void streamStart(int samplecount){
    FILE *comm;      
    fclose(fopen("tmpresults", "w"));                              
    for(int i=0;i<samplecount;i++){                                
        comm = popen(" ../stream/stream-1.3.1/stream-bin >> tmpresults ","r");
        fclose(comm);
    }
}
void streamFinish(){
    remove("../stream/stream-1.3.1/stream-bin");
    resetPar();
}
void qgears2Start(int samplecount){
    FILE *avg,*comm;
    char avgStr[255];
    double avgVal=0;
    int length=0;
    fclose(fopen("tmpresults","w"));
    fclose(fopen("avg.log","w"));
    for(int i=0;i<samplecount;i++){
        system("../qgears2/qgears2-1.0.1/qgears-run.sh > avg.log");//ʘ‿ʘ 
        avg = fopen("avg.log","r");
        while((fscanf(avg,"%s",avgStr))!=EOF){
            avgVal+=atof(avgStr);
            length++;
        }
        comm = fopen("tmpresults","a");
        fprintf(comm,"%.2lf\n",(avgVal/length));
        fclose(comm);
    }     
}

void qgears2Finish(){
    system("make clean --directory=../qgears2/qgears2-1.0.1/");
    resetPar();
}
void piStart(int samplecount){
    FILE *comm;      
    fclose(fopen("tmpresults", "w"));                              
    for(int i=0;i<samplecount;i++){                                
        comm = popen(" /usr/bin/time -o ./tmpresults --append -f '%e' ../pi/pi-benchmark/sample-program.sh ","r");
        fclose(comm);
    }
}
void piFinish(){
    remove("../pi/pi-benchmark/sample-pi-program");
    resetPar();
}
void ebizzyStart(int samplecount){
    FILE *comm;        
    fclose(fopen("tmpresults", "w"));                              
    for(int i=0;i<samplecount;i++){                                
        comm = popen(" ../ebizzy/ebizzy-0.3/ebizzy -S 20 >> tmpresults ","r");
        fclose(comm);
    }
}
void ebizzyFinish(){
    remove("../ebizzy/ebizzy-0.3/ebizzy");
    resetPar();
}
void resetPar(){
    system("sysctl kernel.sched_tunable_scaling=1");
    system("sysctl kernel.sched_latency_ns=12000000");
    system("sysctl kernel.sched_min_granularity_ns=1500000");
    system("sysctl kernel.sched_wakeup_granularity_ns=2000000");
}
void  SIGhandler(int sig)
{
    switch (sig) {
        case SIGINT:
            resetPar();
            exit(0);
        break;
        case SIGTERM:
            resetPar();
            exit(0);
        break; 
    }
}
