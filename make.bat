lcc -c -o tileset.o res/tileset.c
lcc -c -o map.o res/map.c
lcc -c -o title.o res/title.c
lcc -c -o main.o dev/main.c
lcc -o rom/Kingdom.gb main.o tileset.o map.o title.o
del *.o 

bgb.exe rom/Kingdom.gb
pause