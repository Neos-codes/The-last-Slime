#include <gb/gb.h>
#include <rand.h>
#include "libs/chars.h"
#include "Backgrounds/testMap.c"

extern void Scroll_bkg();

// Slime
// ------ Mechanics
extern void Slime_map_move();
extern void Slime_map_collision();
// ------ Animations
extern void Slime_anim_idle();
extern void Slime_anim_moving();
extern void Slime_animMap_handler();
// Enemies
// ------ Mechanics
// Knight
extern void Check_2x1_collisions();
extern void Enemy_2x1_map_move();
// ------ Animations
// Knight
extern void Enemy_anim_idle();
extern void Enemy_anim_moving();
extern void Enemy_animMap_handler();
// Utils
// ----- RANDOM NUMBERS
extern void Set_seed_rand();
// ----- INPUT
extern void Input();
// ----- VBlanks
extern void vbl_update();

//================================
// ----- Global Variables ----- //
//================================
// Slime
extern UINT8 state;         // Para animaciones (0 - 1)
extern UINT8 input;             // Aqui guardamos el input completo
extern UINT8 slime_dir;         // 0 up, 1 down, 2 right, 3 left
extern UINT8 isMoving;      // Flag TRUE si el Slime se esta moviendo
extern UINT8 pixels_moved;      // Cantidad de pixeles (y frames) mientras el slime está en movimiento (son 16 pixeles por movimiento)
extern UINT8 frames_anim;       // Frames de animaciones (ejemplo, 30 frames = 1 cambio de sprite de animacion del Slime en idle)
// Background
extern UINT16 bkg_x, bkg_y;
extern UINT8 scroll;
// Utils
//----- Iterations
extern UINT8 i, j;
//----- Random
//extern UINT8 rand_;
extern UINT16 seed;
//----- VBlanks
extern UINT8 vbl_count;
//========================
// ----- PER STAGE -----//
//========================

// Slime "instance" (struct)
extern struct Slime player;

// Enemies "instances" (struct)
//extern struct Enemy knight;
extern struct Enemy *auxEnemy;

extern struct Enemy enemies_array[5];
extern UINT8 nEnemies;


//=====================================
//---------Internal Functions----------
//=====================================

// ----- Definiciones de funciones

// Donde funciona el juego en los mapas
void Gameloop();
// Escoge una dirección estandar para los enemigos
void Enemy_Choose_dir();
// A menos de un radio "r" de distancia, persigue al jugador
void Enemy_Closest_dir();


// ------ Implementacion de funciones
void Gameloop() {

  //set_bkg_tiles(0, 0, 30, 30, testBkg);

  // Loop de juego
    while(1){

        if(!vbl_count)
            wait_vbl_done();
        vbl_count = 0;

        
        // Solo se puede recibir input cuando slime está quieto
        if(!isMoving)
            input = joypad();
        
        Slime_map_move();
        
        // Animaciones del Knight
        Enemy_animMap_handler();

        // Animaciones del Slime
        Slime_animMap_handler();


        // Para frames de animacion
        frames_anim++;

    }
}

void Enemy_Choose_dir(){
    // Si aun le quedan pasos por dar en su direccion actual
    if(auxEnemy -> steps != 10){
        UINT8 k;
        // Revisar si aun se puede mover en esa direccion
        for(k = 0; k < 4; k++){
            Check_2x1_collisions();
            // Si no se puede mover, cambiar direccion
            if(!auxEnemy -> isMoving){
                auxEnemy -> dir = (auxEnemy -> dir + 1) % 4;
                auxEnemy -> steps = 0;
            }
            // Si se puede seguir en su direccion actual, aumentar 1 paso
            else{
                auxEnemy -> steps++;
                return;
            }
        }
    }
    // Si ya no le quedan pasos, cambiar direccion
    else{
        UINT8 k;
        for(k = 0; k < 4; k++){
            auxEnemy -> dir = (auxEnemy -> dir + 1) % 4;
            Check_2x1_collisions();
            if(auxEnemy -> isMoving){
                auxEnemy -> steps = 0;
                break;
            }
        }
    }
}

void Enemy_Closest_dir(){
    UINT8 rand_ = rand() % 2;

    while(1){
        if(rand_){
            if(auxEnemy -> x > player.x){
                // Hacia la izquierda
                auxEnemy -> dir = 3;
                break;
            }
            else if(auxEnemy -> x < player.x){
                // Hacia la derecha
                auxEnemy -> dir = 2;
                break;
            }
        }
        else{
            if(auxEnemy -> y > player.y){
                // Hacia abajo
                auxEnemy -> dir = 1;
                break;
            }
            else if(auxEnemy -> y < player.y){
                // Hacia arriba
                auxEnemy -> dir = 0;
                break;
            }
        }
        rand_ = !rand_;
    }
}