cat "$1" | grep "records/s" >> calc;
awk '{print $1} NR % 5 == 0 {printf"\n"}' calc | less
