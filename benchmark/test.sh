#!/bin/bash
#Készítette Vass Dávid Attila
sysctl kernel.sched_tunable_scaling=0
#Ezt  a tunable_scaling paramétert, 0-ra kell hogy állítsam mivel ez a beállított értékeimen még módosítani.
#Emiatt a teszt nem lenne eredményes.
#A programban kezdetben négy részre vannak szétszedve a beállítási paraméterek és minden beállítással 5 mintát veszek.
#Ebből remélhetőleg lehet majd látni a változásokat.
#A program kimenete: <latency> <min_gran> <wakeup_gran> <prio>
#                    <time parancs átlagos kimenete 5x>//real time, user time, sys time
#A szebb kimenet érdekében, ezt módosítani fogom.
latency=0;
min_gran=0;
wakeup_gran=0;
prio=0;
setPrio=0;
#Két prioritás változót használok mivel ezt nem állíthatom sysctl segítségével.
#Az első változó a beállítások iterációjához szükséges, a setPrio pedig a konkrét prioritással futtatja a programot.
setLatency() {
    case $1 in
        0)
            sysctl kernel.sched_latency_ns=100000
            ;;
        1)
            sysctl kernel.sched_latency_ns=333400000
            ;;
        2)
            sysctl kernel.sched_latency_ns=666700000
            ;;
        3)
            sysctl kernel.sched_latency_ns=1000000000
            ;;
        *)
            echo "ERROR!set_latency" >> log
    esac
}
setMin_gran() {
    case $1 in
        0)
            sysctl kernel.sched_min_granularity_ns=100000
            ;;
        1)
            sysctl kernel.sched_min_granularity_ns=333400000
            ;;
        2)
            sysctl kernel.sched_min_granularity_ns=666666666
            ;;
        3)
            sysctl kernel.sched_min_granularity_ns=1000000000
            ;;
        *)
            echo "ERROR!set_min_gran" >> log
    esac
}
setWakeup_gran() {
    case $1 in
        0)
            sysctl kernel.sched_wakeup_granularity_ns=0
            ;;
        1)
            sysctl kernel.sched_wakeup_granularity_ns=333333333
            ;;
        2)
            sysctl kernel.sched_wakeup_granularity_ns=666666666
            ;;
        3)
            sysctl kernel.sched_wakeup_granularity_ns=1000000000
            ;;
        *)
            echo "ERROR!setWakeup_gran" >> log
    esac
}
for latency in {0..3}
do
    for min_gran in {0..3}
    do
        for wakeup_gran in {0..3}
        do
            for prio in {0..3}
            do
                echo $latency:$min_gran:$wakeup_gran:$prio
                echo "************************************************" >> log
                setLatency $latency
                setMin_gran $min_gran
                setWakeup_gran $wakeup_gran
                echo $latency $min_gran $wakeup_gran $prio >> log
                case $prio in
                    0)
                        setPrio=-20
                        ;;
                    1)
                        setPrio=-6
                        ;;
                    2)
                        setPrio=6
                        ;;
                    3)
                        setPrio=19
                        ;;
                    *)
                        echo "ERROR!setPrio" >> log
                esac
                { time nice -$setPrio ./sample-program; } &>>log 
                { time nice -$setPrio ./sample-program; } &>>log 
                { time nice -$setPrio ./sample-program; } &>>log 
                { time nice -$setPrio ./sample-program; } &>>log 
                { time nice -$setPrio ./sample-program; } &>>log 
            done
 
        done

    done

done

