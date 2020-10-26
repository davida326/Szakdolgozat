# Pi számító benchmark

A benchmark programot mellékeltem, a `pi-batch-run.sh` script indítja el a benchmarkot, a megfelelő ütemező beállításokkal és ez outputot egy `ertekek.log` nevű fájlba menti.

Annak érdekében hogy a sysctl-t és a programok nice értékét módosítani lehessen, a `pi-batch-run.sh`-t root jogosultsággal kell futtatni.

```bash
	$ sudo ./test.sh
```

Minden beállítással öt darab mintát vesz, az output megjelenítésén még módosítanom kellesz.

