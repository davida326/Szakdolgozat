#include "parameter-test.h"
#include "xmlparser.h"


/*    Parameter-test program az ütemező hangoló változók tesztelésére.
    A program müködése: -Makefile && sudo ./parameter-test
                        -Teszt választása
                        -SampleCount megadása, azaz a kívánt mintaszám.
                            Ezzel a pontosság növelhető, viszont több időt vesz igénybe.
                            Ha nem adunk meg számot, csak Enter-t nyomunk, 5 mintát fog venni tesztenként.
                        -parameter split megadása, itt azt adjuk meg hogy a tesztelni kívánt paraméterek intervallumát, hány részre szedjük szét
                            Egyetlen enter megnyomása esetén, alapbeállítás 5.
                        -Amennyiben létezik a teszthez, a szükséges konfigurációs beállítások megadása számokkal (0-N)
                        -A program futása során, nincs lehetőség részeredmények mentésére.
                        -Kimenet: egyetlen .json fájl a ./logs jegyzékben

*/


int main(int argc, char *argv[]){
    initscr();                 //ncurses screen initialisation 
    raw();                      
    int intervalSplit,sampleCount;
    char testName[250]="";
    selectedTest(&sampleCount,&intervalSplit,testName); // tesztek, hozzátartozó változók beállítása
    preparations(testName);                             // config fájlok írása
    startTest(intervalSplit,sampleCount,testName);      // a teszt indítása, ütemező hangoló paramétereket változtatásával
    endwin();               
    
    return 0;
}
