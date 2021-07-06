// Utils
#include <gb/gb.h>
#include <stdio.h>
#include <rand.h>
// Sprites
#include "Sprites/slime.c"
#include "Sprites/enemies.c"
// Background tiles
#include "Background_Tiles/test_bkg.c"
// Backgrounds
#include "Backgrounds/testBkg.c"

//========================
// ----- Structs ----- //
//========================

// Slime struct
typedef struct{
    UINT8 hp;     // Hp of Slime
    UINT8 x, y;   // Slime position (x, y)
}Slime;

// Enemy Struct
typedef struct{
    unsigned char type;   // Type of enemy: Rookie, Monster, etc
    UINT8 stamina;        // Enemy stamina before get tired
    UINT8 x, y;           // Enemy position (x, y)
}Enemy;


//============================
// ----- Functions ----- //
//===========================

// Slime
// ------ Mechanics
void Slime_map_move();
// ------ Animations
void Slime_anim_idle();
void Slime_anim_moving();
void Slime_animMap_handler();
// Enemies
// ------ Mechanics
// Knight
void Knight_map_move();
// ------ Animations
// Knight
void Knight_anim_idle();
void Knight_anim_moving();
void Knight_animMap_handler();
// Utils
// ----- RANDOM NUMBERS
void Set_seed_rand();
// ----- INPUT
void Input();
// ----- VBlanks
void vbl_update();

//================================
// ----- Global Variables ----- //
//================================
// Slime
UINT8 state = FALSE;         // Para animaciones (0 - 1)
UINT8 input = 0;             // Aqui guardamos el input completo
UINT8 slime_dir = 0;         // 0 up, 1 down, 2 right, 3 left
UINT8 isMoving = FALSE;      // Flag TRUE si el Slime se esta moviendo
UINT8 pixels_moved = 0;      // Cantidad de pixeles (y frames) mientras el slime está en movimiento (son 16 pixeles por movimiento)
UINT8 frames_anim = 0;       // Frames de animaciones (ejemplo, 30 frames = 1 cambio de sprite de animacion del Slime en idle)
// Utils
//----- Random
UINT8 rand_;
UINT16 seed;
//----- VBlanks
UINT8 vbl_count = 0;
//========================
// ----- PER STAGE -----//
//========================

// Slime "instance" (struct)
Slime player;

// Enemies "instances" (struct)
Enemy knight;


void main(){

    // Inicializar parametros de personajes
    player.x = 9;   // para pruebas
    player.y = 56;

    // Inicializar parametros de enemigos
    knight.x = 50;
    knight.y = 50;


    // Inicializar parametros de animaciones
    state = FALSE;
    frames_anim = FALSE;
    // De movimientos
    pixels_moved = 0;
    isMoving = FALSE;
    // Inicializar parametros de RANDOM
    rand_ = 0;
    seed = 0;

    //==========================
    // Set background tiles
    //==========================
    set_bkg_data(0, 4, test_bkg);
    set_bkg_tiles(0, 0, 30, 30, testBkg);

    //==========================
    // Set Slime Sprites
    //==========================
    set_sprite_data(0, 9, slime);
    set_sprite_tile(0, 0);
    set_sprite_tile(1, 1);
    set_sprite_tile(2, 2);
    set_sprite_tile(3, 3);
    // Set Slime Position
    move_sprite(0, 9, 64);
    move_sprite(1, 9, 56);
    move_sprite(2, 17, 56);
    move_sprite(3, 17, 64);

    //=========================
    // Set enemy test Sprites
    //=========================
    set_sprite_data(9, 3, Enemies);
    set_sprite_tile(4, 9);
    set_sprite_tile(5, 10);
    move_sprite(4, 50, 50);
    move_sprite(5, 50, 58);

    // Flag sprite
    set_sprite_tile(15, 4);
    move_sprite(15, 0x8, 0x10);


    // Para fluidez
    disable_interrupts();
    add_VBL(vbl_update);
    add_VBL(Set_seed_rand);
    set_interrupts(VBL_IFLAG);
    enable_interrupts();

    SHOW_SPRITES;
    SHOW_BKG;
    DISPLAY_ON;


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
        Knight_animMap_handler();

        // Animaciones del Slime
        Slime_animMap_handler();


        // Para frames de animacion
        frames_anim++;

    }
}

void Slime_map_move(){

    // Si no se esta moviendo, abierto a recibir inputs de direccion
    if(!isMoving){
        if(input & J_RIGHT || input & J_LEFT || input & J_UP || input & J_DOWN){
            isMoving = TRUE;
            slime_dir = input;   // Se guarda el ultimo input de direccion
            // ----- Coordinacion de animaciones
            // Reinicia los contadores para animar los pasos del slime y los npc's
            frames_anim = 0;
            state = FALSE;
            // AQUI VA EL CAMBIAR (X, Y) DEL SLIME
            // Get random number for enemy directions moving
            rand_ = ((UINT8)rand()) % (UINT8)4;
            //printf("rand = %u", rand_);
            // ------ TESTING
            scroll_sprite(15, 1, 1);  // Flag de captura de movimiento
        }
    }
    
    // Si se está moviendo, terminar de mover 8 pixeles con la ultima direccion obtenida
    if(isMoving){
        Knight_map_move();
        if(slime_dir & J_RIGHT){
            scroll_sprite(0, 1, 0);
            scroll_sprite(1, 1, 0);
            scroll_sprite(2, 1, 0);
            scroll_sprite(3, 1, 0);
            // TO DO: Esto está mal, deberia cambiar una vez. Debe ir en la condición de arriba if(!isMoving)
            player.x++;  // Mover en coordenadas del mapa
        }
        else if(slime_dir & J_LEFT){
            scroll_sprite(0, -1, 0);
            scroll_sprite(1, -1, 0);
            scroll_sprite(2, -1, 0);
            scroll_sprite(3, -1, 0);
            player.x--;   // Mover en coordenadas del mapa
        }
        else if(slime_dir & J_UP){
            scroll_sprite(0, 0, -1);
            scroll_sprite(1, 0, -1);
            scroll_sprite(2, 0, -1);
            scroll_sprite(3, 0, -1);
            player.y--;   // Mover en coordenadas del mapa
        }
        else if(slime_dir & J_DOWN){
            scroll_sprite(0, 0, 1);
            scroll_sprite(1, 0, 1);
            scroll_sprite(2, 0, 1);
            scroll_sprite(3, 0, 1);
            player.y++;    // Mover en coordenadas del mapa
        }

        // Se mueve por defecto 16 pixeles
        pixels_moved++;
        if(pixels_moved == 16){
            slime_dir = 0;
            pixels_moved = 0;
            isMoving = FALSE;
            // ----- Coordinacion de animacion
            // Reinicia los contadores para pasar a animar el idle del Slime y los npc's
            frames_anim = 0;
            state = FALSE;
            // ----------------------------
        }
        // Una vez se mueve los 16 pixeles, se puede recibir input de movimiento nuevamente

    }
}

void Set_seed_rand(){
    seed++;
    initrand(seed);
    if(seed == 255)
        seed = 0;
}

void Slime_anim_idle(){

    if(frames_anim == 30){
        if(state){
            set_sprite_tile(0, 4);
            set_sprite_tile(1, 5);
            set_sprite_tile(2, 6);
            set_sprite_tile(3, 7);
            state = FALSE;
        }
        else{
            set_sprite_tile(0, 0);
            set_sprite_tile(1, 1);
            set_sprite_tile(2, 2);
            set_sprite_tile(3, 3);
            state = TRUE;
        }
        frames_anim = 0;
    }
}

void Slime_anim_moving(){

    if(frames_anim == 3){
        if(state){
            set_sprite_tile(0, 8);
            set_sprite_tile(1, 5);
            set_sprite_tile(2, 6);
            set_sprite_tile(3, 7);
            state = FALSE;
        }
        else{
            set_sprite_tile(0, 4);
            set_sprite_tile(1, 5);
            set_sprite_tile(2, 6);
            set_sprite_tile(3, 7);
            state = TRUE;
        }
        frames_anim = 0;
    }
}

void Slime_animMap_handler(){
    if(isMoving)
        Slime_anim_moving();
    else
        Slime_anim_idle();
}

void Knight_map_move(){
    // Mover arriba
    if(rand_ == 0){
        scroll_sprite(4, 0, -1);
        scroll_sprite(5, 0, -1);
        knight.y -= 1;
    }
    // Mover abajo
    else if(rand_ == 1){
        scroll_sprite(4, 0, 1);
        scroll_sprite(5, 0, 1);
        knight.y += 1;
    }
    // Mover Derecha
    else if(rand_ == 2){
        // BUG
        scroll_sprite(4, 1, 0);
        scroll_sprite(5, 1, 0);
        knight.x += 1;
    }
    else{
        //BUG
        scroll_sprite(4, -1, 0);
        scroll_sprite(5, -1, 0);
        knight.x -= 1;
    }
}

void Knight_anim_idle(){
    if(frames_anim == 30){
        if(state){
            set_sprite_tile(4, 11);
            set_sprite_tile(5, 10);
        }
        else{
            set_sprite_tile(4, 9);
            set_sprite_tile(5, 10);
        }
    }
}

void Knight_anim_moving(){
    if(frames_anim == 3){
        if(state){
            scroll_sprite(4, 0, -1);
            scroll_sprite(5, 0, -1);
        }
        else{
            scroll_sprite(5, 0, 1);
            scroll_sprite(4, 0, 1);
        }
    }
}

void Knight_animMap_handler(){
    if(isMoving)
        Knight_anim_moving();
    else
        Knight_anim_idle();
}


void vbl_update(){
    vbl_count++;
}