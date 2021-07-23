#include <gb/gb.h>
#include "Backgrounds/testBattle.c"


// ----- VBlanks
extern void vbl_update();

extern UINT8 vbl_count;

// ----- Banks
extern UINT8 actualBank;

void BattleLoop(){

    set_bkg_tiles(0, 0, 20, 18, batteMap_test);

    while(1){

        if(!vbl_count)
            wait_vbl_done();
        vbl_count = 0;
    }
}