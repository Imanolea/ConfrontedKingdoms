lcc -c -o tileset.o res/tileset.c
lcc -c -o main.o dev/main.c
lcc -o rom/Kingdom.gb main.o tileset.o
del *.o 
bgb.exe rom/Kingdom.gb
pause