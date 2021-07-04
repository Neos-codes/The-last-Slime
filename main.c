#include <gb/gb.h>
#include "Sprites/slime.c"

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
void Slime_move();
// ------ Animations
void Slime_anim_idle();
void Slime_anim_moving();
void Slime_animMap_handler();
// Utils
// ----- INPUT
void Input();
// ----- VBlanks
void vbl_update();

//================================
// ----- Global Variables ----- //
//================================
// Slime
UINT8 state = FALSE;        // For animations (0 - 1)
UINT8 input = 0;
UINT8 slime_dir = 0;    // 0 up, 1 down, 2 right, 3 left
UINT8 isMoving = FALSE;     // If slime is moving
UINT8 pixels_moved = 0; // number of frames that slime is moving by input
UINT8 frames_anim = 0;  // Use for animations (example, 20 frames = 1 state of sprite in idle_slime)
// Utils
//----- VBlanks
UINT8 vbl_count = 0;
//========================
// ----- PER STAGE -----//
//========================

// Slime "instance" (struct)
Slime player;


void main(){

    // Inicializar parametros de personajes
    player.x = 0;   // para pruebas
    player.y = 4;

    // Inicializar parametros de animaciones
    state = FALSE;
    frames_anim = FALSE;
    // De movimientos
    pixels_moved = 0;
    isMoving = FALSE;

    // Set Slime Sprites
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

    // Flag sprite
    set_sprite_tile(4, 4);
    move_sprite(4, 0x8, 0x10);


    // Para fluidez
    disable_interrupts();
    add_VBL(vbl_update);
    set_interrupts(VBL_IFLAG);
    enable_interrupts();

    SHOW_SPRITES;

    // Loop de juego
    while(1){

        if(!vbl_count)
            wait_vbl_done();
        vbl_count = 0;


        // Solo se puede recibir input cuando slime está quieto
        if(!isMoving)
            input = joypad();
        Slime_move();
        
        // Animaciones del Slime
        Slime_animMap_handler();
        // Para frames de animacion
        frames_anim++;

    }

}

void Slime_move(){

    // Si no se esta moviendo, abierto a recibir inputs de direccion
    if(!isMoving){
        if(input & J_RIGHT || input & J_LEFT || input & J_UP || input & J_DOWN){
            isMoving = TRUE;
            slime_dir = input;   // Se guarda el ultimo input de direccion
            // ----- Coordinacion de animaciones
            // Reinicia los contadores para animar los pasos del slime y los npc's
            frames_anim = 0;
            state = FALSE;
            // ------ TESTING
            scroll_sprite(4, 1, 1);  // Flag de captura de movimiento
        }
    }
    
    // Si se está moviendo, terminar de mover 8 pixeles con la ultima direccion obtenida
    if(isMoving){
        if(slime_dir & J_RIGHT){
            scroll_sprite(0, 1, 0);
            scroll_sprite(1, 1, 0);
            scroll_sprite(2, 1, 0);
            scroll_sprite(3, 1, 0);
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

    if(frames_anim == 2){
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

void vbl_update(){
    vbl_count++;
}