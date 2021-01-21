#include "parameter-test.h"
#include "xmlparser.h"

int main(int argc, char *argv[]){
   
    int intervalSplit,sampleCount;
    int testNum = selectedTest(&sampleCount,&intervalSplit);
    endwin();
    preparations();
    startTest(testNum,intervalSplit,sampleCount);
     /*
    char timeStamp[256] = {0};
    time_t rawtime = time(NULL);
    struct tm *ptm = localtime(&rawtime);
    strftime(timeStamp, 256, "%Y%m%d_%T-results.json", ptm);
    char mesg[255];
    strcpy(mesg,timeStamp);
    FILE *fp = fopen(mesg,"w");


    fclose(fp);
    */
    return 0;
}