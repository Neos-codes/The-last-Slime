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
// Posicion de la celda en que se encuentra el slime (0 centro, 1 arriba, 2 abajo, 3 atrás)
UINT8 slime_movement_frames;
UINT8 avoiding;

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
    slime_movement_frames = 0;

    // Inicializar parametros para animaciones

    frames_anim = 0;
    state = FALSE;
    isMoving = FALSE;
    avoiding = FALSE;
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

        // Recibir botones
        input = joypad();

        // Tomar movimientos de Slime
        if(!isMoving){
            // Escoger dirección del Slime
            Take_Input();
        }

        // Mover Slime
        if(isMoving){
            // Sacar Slime del centro
            if(avoiding){
                Avoid_Slime();
            }
            // Devolver Slime al centro
            else{
                Return_toCenter();   // BUG AQUI?
            }
        }

        frames_anim++;
    } // ENDWHILE
}

void Take_Input(){
    // Hacia arriba
    if(input & J_UP){
        isMoving = TRUE;
        avoiding = TRUE;
        slime_dir = 1;
    } // Hacia abajo
    else if(input & J_DOWN){
        isMoving = TRUE;
        avoiding = TRUE;
        slime_dir = 2;
    } // Hacia atrás
    else if(input & J_LEFT){
        isMoving = TRUE;
        avoiding = TRUE;
        slime_dir = 3;
    }
}

void Avoid_Slime(){
    // Hacia arriba
    if(slime_dir == 1){
        scroll_sprite(0, 0, -1);
        scroll_sprite(1, 0, -1);
        scroll_sprite(2, 0, -1);
        scroll_sprite(3, 0, -1);
    }
    else if(slime_dir == 2){
        // Hacia abajo
        scroll_sprite(0, 0, 1);
        scroll_sprite(1, 0, 1);
        scroll_sprite(2, 0, 1);
        scroll_sprite(3, 0, 1);
    }
    else if(slime_dir == 3){
        // Hacia la izquierda
        scroll_sprite(0, -1, 0);
        scroll_sprite(1, -1, 0);
        scroll_sprite(2, -1, 0);
        scroll_sprite(3, -1, 0);
    }
    // Aumentar contador de pixeles movidos
    slime_movement_frames++;

    // Si se movió 32 pixeles, vuelve a 0 y a su posicion central
    if(slime_movement_frames == 32){
        slime_movement_frames = 0;
        avoiding = FALSE;
    }
}

void Return_toCenter(){
    // Si esta arriba, volver hacia abajo
    if(slime_dir == 1){
        scroll_sprite(0, 0, 1);
        scroll_sprite(1, 0, 1);
        scroll_sprite(2, 0, 1);
        scroll_sprite(3, 0, 1);
    }
    else if(slime_dir == 2){
        // Si esta abajo, volver hacia arriba
        scroll_sprite(0, 0, -1);
        scroll_sprite(1, 0, -1);
        scroll_sprite(2, 0, -1);
        scroll_sprite(3, 0, -1);
    }
    else if(slime_dir == 3){
        // Si esta a la izquierda, volver hacia el centro
        scroll_sprite(0, 1, 0);
        scroll_sprite(1, 1, 0);
        scroll_sprite(2, 1, 0);
        scroll_sprite(3, 1, 0);
    }
    // Aumentar contador de pixeles movidos
    slime_movement_frames++;

    // Si se movió 32 pixeles, vuelve a 0 y a su posicion central
    if(slime_movement_frames == 32){
        slime_movement_frames = 0;
        slime_dir = 0;
        isMoving = FALSE;
    }
}