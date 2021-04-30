# Szakdolgozat

Használati módokra optimalizált processz ütemezési stratégiák

A dolgozat azt vizsgálja, hogy a Linux kernel esetében milyen lehetőségek vannak az ütemezés optimalizálására különféle operációs rendszer felhasználati módok esetében. Tipikusan elkülöníthetjük például az asztali környezettel használt, illetve a szerverként üzemeltetett rendszereket. A dolgozat áttekinti az ismert ütemezéső algoritmusokat. Benchmark feladatok segítségével mérhetővé teszi, hogy az ütemezőkben elvégzett változtatások milyen hatást gyakorolnak a végrehajtáshoz szükséges időre.

Az optimalizáláshoz szükség van adatokra, amelyeket a kernelből kernel modul segítségével lehet kinyerni. Az így kapott adatok vizsgálata alapján lehet javaslatokat tenni a processzek végrehajtási sorrendjének, időszeletek méretének, illetve a prioritások megváltoztatására vonatkozóan.

A kernel modul C programozási nyelven készül. Az adatok elemzése, megjelenítése Python segítségével történik.

