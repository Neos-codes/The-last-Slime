#include <gb/gb.h>
#include "libs/chars.h"
#include "Backgrounds/testBattle.c"

// Structs
extern struct Slime player;
extern struct Enemy *enemies_array;
extern struct Enemy *auxEnemy;

// Sobre personajes
extern UINT8 nEnemies;
extern UINT8 battleEnemy_index;
// ----- VBlanks
extern void vbl_update();

extern UINT8 vbl_count;

// ----- Banks
extern UINT8 actualBank;


// Internal shit
void IniBattle(){
    UINT8 eSprite;
    //auxEnemy = &enemies_array[eIndex];
    eSprite = auxEnemy -> sprite;
    // Posicionar slime en la casilla central
    move_sprite(0, 48, 88);
    move_sprite(1, 48, 80);
    move_sprite(2, 56, 80);
    move_sprite(3, 56, 88);

    
    // Posicionar enemigo en su casilla
    move_sprite(eSprite, 112, 80);
    move_sprite(eSprite + 1, 112, 88);
    if(auxEnemy -> type == 's'){
        move_sprite(eSprite + 2, 120, 80);
        move_sprite(eSprite + 3, 120, 88);

    }
    


}

void BattleLoop(){

    set_bkg_tiles(0, 0, 20, 18, batteMap_test);

    IniBattle();

    while(1){

        if(!vbl_count)
            wait_vbl_done();
        vbl_count = 0;

    
        if(joypad() & J_START){
            actualBank = 1;
            return;
        }
    }
}