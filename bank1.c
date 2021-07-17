#include <gb/gb.h>
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

extern struct Enemy enemies_array[5];
extern UINT8 nEnemies;


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