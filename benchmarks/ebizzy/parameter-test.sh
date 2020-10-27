#!/bin/bash
#Függőségek "time" program
#Készítette Vass Dávid Attila
sysctl kernel.sched_tunable_scaling=0
#Ezt  a tunable_scaling paramétert, 0-ra kell hogy állítsam mivel ez a beállított értékeimen még módosítani.
#Emiatt a teszt nem lenne eredményes.
#A programban kezdetben négy részre vannak szétszedve a beállítási paraméterek és minden beállítással 5 mintát veszek.
#Ebből remélhetőleg lehet majd látni a változásokat.
#Az ebizzy program, kimenetéhez a program alapból hozzáfűzte az eltelt időt, de én ezt beállítom egy paraméterrel. Emiatt a forráskódból kikommenteztem.
#Az eredmények itt kérések számát jelzik, amennyit 20mp végre tudott hajtani. (Processzor igényes - Webserver workload)
#A program kimenete: JSON fájlt: ertekek.json
latency=0;
min_gran=0;
wakeup_gran=0;
prio=0;
setPrio=0;
#Két prioritás változót használok mivel ezt nem állíthatom sysctl segítségével.
#Az első változó a beállítások iterációjához szükséges, a setPrio pedig a konkrét prioritással futtatja a programot.
echo '{
    "measurements": [' &>>ertekek.json
setLatency() {
    local -n ref=$2
    case $1 in
        0)
            ref=100000
            sysctl kernel.sched_latency_ns=100000
            ;;
        1)
            ref=333400000
            sysctl kernel.sched_latency_ns=333400000
            ;;
        2)
            ref=666700000
            sysctl kernel.sched_latency_ns=666700000
            ;;
        3)
            ref=1000000000
            sysctl kernel.sched_latency_ns=1000000000
            ;;
        *)
            echo "ERROR!set_latency" >>ertekek.json
    esac
}
setMin_gran() {
    local -n ref=$2
    case $1 in
        0)
            ref=100000
            sysctl kernel.sched_min_granularity_ns=100000
            ;;
        1)
            ref=333400000
            sysctl kernel.sched_min_granularity_ns=333400000
            ;;
        2)
            ref=666666666
            sysctl kernel.sched_min_granularity_ns=666666666
            ;;
        3)
            ref=100000000
            sysctl kernel.sched_min_granularity_ns=1000000000
            ;;
        *)
            echo "ERROR!set_min_gran" >>ertekek.json
    esac
}
setWakeup_gran() {
    local -n ref=$2
    case $1 in
        0)
            ref=0
            sysctl kernel.sched_wakeup_granularity_ns=0
            ;;
        1)
            ref=333333333
            sysctl kernel.sched_wakeup_granularity_ns=333333333
            ;;
        2)
            ref=666666666
            sysctl kernel.sched_wakeup_granularity_ns=666666666
            ;;
        3)
            ref=1000000000
            sysctl kernel.sched_wakeup_granularity_ns=1000000000
            ;;
        *)
            echo "ERROR!setWakeup_gran" >>ertekek.json
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
                setLatency $latency realLatency
                setMin_gran $min_gran realMin_gran
                setWakeup_gran $wakeup_gran realWakeup_gran
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
                            echo "ERROR!setPrio" >> ertekek.json
                    esac
                    echo '{ "parameters":' &>>ertekek.json
                    echo '{"latency":"'$realLatency'", "min_gran":"'$realMin_gran'", "wakeup_gran":"'$realWakeup_gran'","prio:":"'$setPrio'"},"results":[' &>>ertekek.json
                    { nice -$setPrio ./ebizzy-0.3/ebizzy -S 20; } &>>ertekek.json 
                    echo ', ' &>>ertekek.json 
                    { nice -$setPrio ./ebizzy-0.3/ebizzy -S 20; } &>>ertekek.json 
                    echo ', ' &>>ertekek.json 
                    { nice -$setPrio ./ebizzy-0.3/ebizzy -S 20; } &>>ertekek.json 
                    echo ', ' &>>ertekek.json 
                    { nice -$setPrio ./ebizzy-0.3/ebizzy -S 20; } &>>ertekek.json 
                    echo ', ' &>>ertekek.json 
                    { nice -$setPrio ./ebizzy-0.3/ebizzy -S 20; } &>>ertekek.json
                    echo ']}' &>>ertekek.json 
                    if [ $latency -eq 1 ] && [ $min_gran -eq 1 ] && [ $wakeup_gran -eq 1 ] && [ $prio -eq 1 ]
                      then echo ""
                      else echo "," &>> ertekek.json
                fi
   		    done
 
        done

    done

done
echo ']}' &>>ertekek.json

