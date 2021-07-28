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
extern UINT8 pixels_moved;
extern UINT8 avoiding;

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
    pixels_moved = 0;
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

        input = joypad();

        // Solo si no se está moviendo, se toman los inputs para esquivar
        if(!isMoving){
            Take_Input();
        }

        // Mover al slime
        if(isMoving){
            // Moverlo del centro
            if(avoiding){
                Avoid_Slime();
            }
            // Devolverlo al centro
            else{
                Return_toCenter();
            }
        }

        // Debug de posicion
        //move_sprite(15, 8 + 8 * slime_dir, 16);
        
        frames_anim++;

    } // ENDWHILE
}

void Take_Input(){
    if(input & J_UP){
        slime_dir = 1;
        avoiding = TRUE;
        isMoving = TRUE;
    }
    else if(input & J_DOWN){
        slime_dir = 2;
        avoiding = TRUE;
        isMoving = TRUE;
    }
    else if(input & J_LEFT){
        slime_dir = 3;
        avoiding = TRUE;
        isMoving = TRUE;
    }
}

void Avoid_Slime(){
    /* Se mueve 32 pixeles en 16 frames, desde el frame ("pixel") 16 al 31 se queda en
     la celda de esquivar */

    // Esquivar hacia arriba
    if(pixels_moved < 16){
        if(slime_dir == 1){
            scroll_sprite(0, 0, -2);
            scroll_sprite(1, 0, -2);
            scroll_sprite(2, 0, -2);
            scroll_sprite(3, 0, -2);
        }
        // Esquivar hacia abajo
        else if(slime_dir == 2){
            scroll_sprite(0, 0, 2);
            scroll_sprite(1, 0, 2);
            scroll_sprite(2, 0, 2);
            scroll_sprite(3, 0, 2);
        }
        // Esquivar hacia la izquierda
        else if(slime_dir == 3){
            scroll_sprite(0, -2, 0);
            scroll_sprite(1, -2, 0);
            scroll_sprite(2, -2, 0);
            scroll_sprite(3, -2, 0);
        }
    }

    pixels_moved++;

    if(pixels_moved == 32){
        pixels_moved = 0;
        avoiding = FALSE;
    }

}

void Return_toCenter(){
    // Si estaba arriba, volver al centro
    if(slime_dir == 1){
        scroll_sprite(0, 0, 2);
        scroll_sprite(1, 0, 2);
        scroll_sprite(2, 0, 2);
        scroll_sprite(3, 0, 2);
    }
    // Si estaba abajo, volver al centro
    else if(slime_dir == 2){
        scroll_sprite(0, 0, -2);
        scroll_sprite(1, 0, -2);
        scroll_sprite(2, 0, -2);
        scroll_sprite(3, 0, -2);
    }
    // Si estaba a la izquierda, volver al centro
    else if(slime_dir == 3){
        scroll_sprite(0, 2, 0);
        scroll_sprite(1, 2, 0);
        scroll_sprite(2, 2, 0);
        scroll_sprite(3, 2, 0);
    }

    pixels_moved++;

    if(pixels_moved == 16){
        pixels_moved = 0;
        slime_dir = 0;
        isMoving = FALSE;
    }
}



