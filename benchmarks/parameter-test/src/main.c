/*
    Parameter-test program az ütemező hangoló változók tesztelésére.
    A program müködése: -Makefile && sudo ./parameter-test
                        -Teszt választása
                        -SampleCount megadása, azaz a kívánt mintaszám.
                            Ezzel a pontosság növelhető, viszont több időt vesz igénybe.
                            Ha nem adunk meg számot, csak Enter-t nyomunk, 5 mintát fog venni tesztenként.
                        -parameter split megadása, itt azt adjuk meg hogy a tesztelni kívánt paraméterek intervallumát, hány részre szedjük szét
                            Egyetlen enter megnyomása esetén, alapbeállítás 5.
                        -A program futása során, nincs lehetőség részeredmények mentésére.
                        -Kimenet: egyetlen .json fájl a ./logs jegyzékben

*/
#include "parameter-test.h"
#include "xmlparser.h"

int main(int argc, char *argv[]){
   
    int intervalSplit,sampleCount;
    int testNum = selectedTest(&sampleCount,&intervalSplit);
    endwin();
    preparations();
    startTest(testNum,intervalSplit,sampleCount);
    
    return 0;
}