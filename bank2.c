#include <gb/gb.h>
#include "libs/chars.h"
#include "Backgrounds/testBattle.c"


//============================
//------ OBSERVACIONES -------
//============================
// - Si slime_dir = 0, el slime está en el centro


// Structs
extern struct Slime player;
extern struct Enemy *enemies_array;
extern struct Enemy *auxEnemy;

// Sobre personajes
extern UINT8 nEnemies;

// Sobre interacciones
extern UINT8 input;
extern UINT8 isMoving;
extern UINT8 slime_dir;

// Sobre animaciones
extern UINT8 state;
extern UINT8 frames_anim;
// ----- VBlanks
extern void vbl_update();

extern UINT8 vbl_count;

// ----- Banks
extern UINT8 actualBank;

// Funciones externas
extern void Slime_anim_idle();
extern void Enemy_anim_idle();

//============================
//-----Variables Internas-----
//============================



//============================
//---- Funciones internas ----
//============================
// Inicializa los parametros de batalla
void IniBattle();
// Loop de batalla
void BattleLoop();
// Esquivar Slime
void Avoid_Slime();
// Volver al centro luego de esquivar
void Return_toCenter();
// Tomar input para interactuar
void Take_Input();

//========================================
//--- Desarrollo de funciones internas ---
//========================================

void IniBattle(){

    UINT8 eSprite;
    //HideSprites();
    //auxEnemy = &enemies_array[eIndex];
    eSprite = auxEnemy -> sprite;
    // Posicionar slime en la casilla central
    move_sprite(0, 48, 88);
    move_sprite(1, 48, 80);
    move_sprite(2, 56, 80);
    move_sprite(3, 56, 88);

    
    // Posicionar enemigo en su casilla
    if(auxEnemy -> type != 's'){
        // Si no es skeleton, partir en posición normal
        move_sprite(eSprite, 112, 80);
        move_sprite(eSprite + 1, 112, 88);
    }
    else{
        // Para Skeleton, debe partir 1 pixel más abajo
        move_sprite(eSprite, 112, 81);
        move_sprite(eSprite + 1, 112, 88);
        move_sprite(eSprite + 2, 120, 81);
        move_sprite(eSprite + 3, 120, 88);
    }

    // Inicializar parametros de movimiento
    
    slime_dir = 0;      // Slime está en la casilla central

    // Inicializar parametros para animaciones

    frames_anim = 0;
    state = FALSE;
    isMoving = FALSE;
}

void BattleLoop(){

    set_bkg_tiles(0, 0, 20, 18, batteMap_test);

    IniBattle();
    
    while(1){

        if(!vbl_count)
            wait_vbl_done();
        vbl_count = 0;

        Enemy_anim_idle();
        Slime_anim_idle();

        frames_anim++;

    } // ENDWHILE
}