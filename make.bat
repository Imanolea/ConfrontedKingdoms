cd mod2gbt
mod2gbt template.mod song -c 2
cd ..

lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o tileset.o res/tileset.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o main.o dev/main.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o output.o dev/output.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o gbt_player.o dev/gbt_player.s
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o gbt_player_bank1.o dev/gbt_player_bank1.s


lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -Wl-yt1 -Wl-yo4 -Wl-ya0 -o rom/Kingdom.gb main.o output.o gbt_player.o gbt_player_bank1.o tileset.o 



del *.o *.lst

bgb.exe rom/Kingdom.gb
pause