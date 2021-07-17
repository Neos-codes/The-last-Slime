// Utils
#include <gb/gb.h>
//#include <stdio.h>
#include <rand.h>
// Sprites
#include "Sprites/slime.c"
#include "Sprites/enemies.c"
// Background tiles
#include "Background_Tiles/test_bkg.c"
// Backgrounds
//#include "Backgrounds/testMap.c"

extern unsigned char testMap [];
extern void Gameloop();
//========================
// ----- Structs ----- //
//========================

// Slime struct
struct Slime{
    UINT8 hp;     // Hp of Slime
    UINT16 x, y;   // Slime position (x, y)
};

// Enemy Struct
struct Enemy{
    unsigned char type;   // Type of enemy: Rookie, Monster, etc
    UINT8 stamina;        // Enemy stamina before get tired
    UINT16 x, y;          // Enemy position (x, y)
    UINT8 isMoving;       // True when there is no obstacle in his direction
    UINT8 sprite;         // Primer sprite del enemigo
    UINT8 dir;
};


//============================
// ----- Functions ----- //
//===========================

// Background
void Scroll_bkg();

// Slime
// ------ Mechanics
void Slime_map_move();
void Slime_map_collision();
// ------ Animations
void Slime_anim_idle();
void Slime_anim_moving();
void Slime_animMap_handler();
// Enemies
// ------ Mechanics
// Knight
void Check_2x1_collisions();
void Enemy_2x1_map_move();
// ------ Animations
// Knight
void Enemy_anim_idle();
void Enemy_anim_moving();
void Enemy_animMap_handler();
// Utils
// ----- RANDOM NUMBERS
void Set_seed_rand();
// ----- INPUT
void Input();
// ----- VBlanks
void vbl_update();

// External functions
extern void Enemy_Choose_dir();
extern void Enemy_Closest_dir();

//================================
// ----- Global Variables ----- //
//================================
// Slime
UINT8 state;             // Para animaciones (0 - 1)
UINT8 input;             // Aqui guardamos el input completo
UINT8 slime_dir;         // 0 up, 1 down, 2 right, 3 left
UINT8 isMoving;          // Flag TRUE si el Slime se esta moviendo
UINT8 pixels_moved;      // Cantidad de pixeles (y frames) mientras el slime está en movimiento (son 16 pixeles por movimiento)
UINT8 frames_anim;       // Frames de animaciones (ejemplo, 30 frames = 1 cambio de sprite de animacion del Slime en idle)
// Background
UINT16 bkg_x, bkg_y;
UINT8 scroll;
// Utils
// Iteration
UINT8 i, j, k;
//----- Random
//UINT8 rand_;
UINT16 seed;
//----- VBlanks
UINT8 vbl_count;
//========================
// ----- PER STAGE -----//
//========================

// Slime "instance" (struct)
struct Slime player;

// About Enemy "instances" (struct)
//struct Enemy knight;
UINT8 nEnemies;
// Auxiliar enemy
struct Enemy *auxEnemy;

struct Enemy enemies_array[5];


void main(){
    // Inicializar utils
    vbl_count = 0;

    // Inicializar Animaciones
    state = FALSE;
    input = 0;
    slime_dir = 0;
    isMoving = FALSE;
    pixels_moved = 0;
    frames_anim = 0;

    // Inicializar parametros del background
    bkg_x = 0;
    bkg_y = 0;
    scroll = FALSE;

    // Inicializar parametros de personajes
    player.x = 1;   // para pruebas
    player.y = 1;

    // Inicializar parametros de enemigos
    nEnemies = 2;
    // Test knight
    enemies_array[0].x = 5;
    enemies_array[0].y = 5;
    enemies_array[0].sprite = 4;
    enemies_array[0].isMoving = FALSE;
    enemies_array[0].type = 'r';
    // Test Skeleton
    enemies_array[1].x = 3;
    enemies_array[1].y = 7;
    enemies_array[1].sprite = 6;
    enemies_array[1].isMoving = FALSE;
    enemies_array[1].type = 's';


    // Inicializar parametros de animaciones
    state = FALSE;
    frames_anim = FALSE;
    // De movimientos
    pixels_moved = 0;
    isMoving = FALSE;
    // Inicializar parametros de RANDOM
    //rand_ = 0;
    seed = 0;

    //==========================
    // Set background tiles
    //==========================
    set_bkg_data(0, 4, test_bkg);
    set_bkg_tiles(0, 0, 30, 30, testMap);

    //==========================
    // Set Slime Sprites
    //==========================
    set_sprite_data(0, 9, slime);
    set_sprite_tile(0, 0);
    set_sprite_tile(1, 1);
    set_sprite_tile(2, 2);
    set_sprite_tile(3, 3);
    // Set Slime Position
    move_sprite(0, 8 + 8 * player.x, 16 + 8 * (player.y + 1));
    move_sprite(1, 8 + 8 * player.x, 16 + 8 * player.y);
    move_sprite(2, 8 + 8 * (player.x + 1), 16 + 8 * player.y);
    move_sprite(3, 8 + 8 * (player.x + 1), 16 + 8 * (player.y + 1));

    //=========================
    // Set enemy test Sprites
    //=========================
    set_sprite_data(9, 7, Enemies);
    // Knight
    set_sprite_tile(4, 9);
    set_sprite_tile(5, 10);
    move_sprite(4, 8 + 8 * enemies_array[0].x, 16 + 8 * enemies_array[0].y);
    move_sprite(5, 8 + 8 * enemies_array[0].x, 16 + 8 * (enemies_array[0].y + 1));
    // Skeleton
    set_sprite_tile(6, 12);
    set_sprite_tile(7, 13);
    set_sprite_tile(8, 14);
    set_sprite_tile(9, 15);
    move_sprite(6, 8 + 8 * enemies_array[1].x, 16 + 8 * enemies_array[1].y + 1);   // .y + 1 por animacion idle
    move_sprite(7, 8 + 8 * enemies_array[1].x, 16 + 8 * (enemies_array[1].y + 1));
    move_sprite(8, 8 + 8 * (enemies_array[1].x + 1), 16 + 8 * enemies_array[1].y + 1);  // .y + 1 por anim idle
    move_sprite(9, 8 + 8 * (enemies_array[1].x + 1), 16 + 8 * (enemies_array[1].y + 1));

    //====== Flag sprite
    // ---Flag Slime.isMoving
    //set_sprite_tile(15, 4);
    //move_sprite(15, 0x8, 0x10);
    // --- Flag Knight isMoving
    //set_sprite_tile(15, 9);
    //move_sprite(15, 0, 16);
    // --- Flag Skeleton isMoving
    set_sprite_tile(16, 14);
    move_sprite(16, 0, 24);


    // Para fluidez
    disable_interrupts();
    add_VBL(vbl_update);
    add_VBL(Set_seed_rand);
    set_interrupts(VBL_IFLAG);
    enable_interrupts();

    SHOW_SPRITES;
    SHOW_BKG;
    DISPLAY_ON;

    SWITCH_ROM_MBC1(1);
    Gameloop();
}

void Check_scroll_bkg(){
    // 160 debe ser width_map - 80
    // dir = RIGHT &&  MITAD DE PANTALLA  && MEDIA PANTALLA ANTES DEL BORDE DERECHO DEL MAPA
    // BUGS
    if(slime_dir == J_RIGHT && 8 * player.x < 144 && 8 * player.x - bkg_x > 72){
        scroll = TRUE;
        bkg_x += 16;
    }
    // dir = LEFT                  Tope Izquierdo         Distancia player y bkg   
    else if(slime_dir == J_LEFT && player.x * 8 > 72 && player.x * 8 - bkg_x <= 72){
        scroll = TRUE;
        bkg_x -= 16;
    }
    //                             Tope inferior           Distancia player y bkg
    else if(slime_dir == J_DOWN && 8 * player.y < 160 && 8 * player.y - bkg_y > 64){
        scroll = TRUE;
        bkg_y += 16;
    }
    //                             Tope Superior
    else if(slime_dir == J_UP && 8 * player.y > 72 && 8 * player.y - bkg_y < 64){
        scroll = TRUE;
        bkg_y -= 16;
    }
}

void Slime_map_move(){
    // Si no se esta moviendo, abierto a recibir inputs de direccion
    if(!isMoving){
        if(input & J_RIGHT || input & J_LEFT || input & J_UP || input & J_DOWN){
            //isMoving = TRUE;
            // ----- Coordinacion de animaciones
            // Guardar direccion slime
            if(input & J_RIGHT) slime_dir = 0x01U;
            else if(input & J_LEFT) slime_dir = 0x02U;
            else if(input & J_UP) slime_dir = 0x04U;
            else if(input & J_DOWN) slime_dir = 0x08U;
       
            // Revisar si se puede mover en la dirección seleccionada
            Slime_map_collision();
            
            // Reiniciar los contadores para animar los pasos del slime y los npc's   
            if(isMoving){
                // Generar una direccion random para los enemigos
                //rand_ = ((UINT8)rand()) % (UINT8)4;
                //printf("rand = %u", rand_);
                for(i = 0; i < nEnemies; i++){
                    auxEnemy = &enemies_array[i];
                    //auxEnemy -> dir = ((UINT8)rand()) % (UINT8)4;
                    auxEnemy -> dir = 3;
                    Check_2x1_collisions();
                }
                frames_anim = 0;
                //state = FALSE;
            }
            // ------ TESTING
            //if(isMoving)
            //    scroll_sprite(15, 1, 1);  // Flag de captura de movimiento
        }
    }
    
    // Si se está moviendo, terminar de mover 8 pixeles con la ultima direccion obtenida
    if(isMoving){
        for(i = 0; i < nEnemies; i++){
            auxEnemy = &enemies_array[i];
            Enemy_2x1_map_move();
        }
        
        if(slime_dir & J_RIGHT){
            if(!scroll){
                scroll_sprite(0, 1, 0);
                scroll_sprite(1, 1, 0);
                scroll_sprite(2, 1, 0);
                scroll_sprite(3, 1, 0);
            }
            else
                scroll_bkg(1, 0);
        }
        else if(slime_dir & J_LEFT){
            if(!scroll){
                scroll_sprite(0, -1, 0);
                scroll_sprite(1, -1, 0);
                scroll_sprite(2, -1, 0);
                scroll_sprite(3, -1, 0);
            }
            else
                scroll_bkg(-1, 0);
        }
        else if(slime_dir & J_UP){
            if(!scroll){
                scroll_sprite(0, 0, -1);
                scroll_sprite(1, 0, -1);
                scroll_sprite(2, 0, -1);
                scroll_sprite(3, 0, -1);
            }
            else
                scroll_bkg(0, -1);
        }
        else if(slime_dir & J_DOWN){
            if(!scroll){
                scroll_sprite(0, 0, 1);
                scroll_sprite(1, 0, 1);
                scroll_sprite(2, 0, 1);
                scroll_sprite(3, 0, 1);
            }
            else
                scroll_bkg(0, 1);
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
            //state = FALSE;
            // ----------------------------
            // scroll bkg flag vuelve a ser falso
            scroll = FALSE;
        }
        // Una vez se mueve los 16 pixeles, se puede recibir input de movimiento nuevamente

    }
}

void Slime_map_collision(){
    if(slime_dir == 0x04U){   // UP
        if(testMap[player.x + 30 * (player.y - 1)] != 0x03){
            isMoving = TRUE;
            Check_scroll_bkg();
            player.y -= 2;
            //bkg_y -= 8;     // FIXED
            return;
        }
    }
    else if(slime_dir == 0x08U){  // DOWN
        if(testMap[player.x + 30 * (player.y + 2)] != 0x03){
            isMoving = TRUE;
            Check_scroll_bkg();
            player.y += 2;
            //bkg_y += 8;     // FIXED
            return;
        }
    }
    else if(slime_dir == 0x01U){  // RIGHT
        if(testMap[player.x + 2 + 30 * player.y] != 0x03){
            isMoving = TRUE;
            // Si está a la mitad de la pantalla, se mueve el mapa no el slime
            Check_scroll_bkg();
            player.x += 2;
            //bkg_x += 8;     // FIXED
            return;
        }
    }
    else if(slime_dir == 0x02U){  // LEFT
        if(testMap[player.x - 1 + 30 * player.y] != 0x03){
            isMoving = TRUE;
            // Si está a la mitad de la pantalla, se mueve el mapa no el slime
            Check_scroll_bkg();
            player.x -= 2;
            //bkg_x -= 8;     // FIXED
            return;
        }
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
            state = FALSE;
        }
        else{
            set_sprite_tile(0, 4);
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

void Check_2x1_collisions(){
    if(auxEnemy -> dir == 0){ // UP
        if(testMap[auxEnemy -> x + 30 * (auxEnemy -> y - 1)] != 0x03){  // BIEN
            auxEnemy -> isMoving = TRUE;
            auxEnemy -> y -= 2;
        }
        else
            auxEnemy -> isMoving = FALSE;
    }
    else if(auxEnemy -> dir == 1){ // DOWN
        if(testMap[auxEnemy -> x + 30 * (auxEnemy -> y + 2)] != 0x03){  // BIEN
            auxEnemy -> isMoving = TRUE;
            auxEnemy -> y += 2;
        }
        else
            auxEnemy -> isMoving = FALSE;
    }
    else if(auxEnemy -> dir == 2){ // RIGHT
        if(testMap[auxEnemy -> x + 2 + 30 * auxEnemy -> y] != 0x03){
            auxEnemy -> isMoving = TRUE;
            auxEnemy -> x += 2;
        }
        else
            auxEnemy -> isMoving = FALSE;
    }
    else if(auxEnemy -> dir == 3){ // LEFT
        if(testMap[auxEnemy -> x - 1 + 30 * auxEnemy -> y] != 0x03){
            auxEnemy -> isMoving = TRUE;
            auxEnemy -> x -= 2;
        }
        else
            auxEnemy -> isMoving = FALSE;
    }
    //knight.isMoving = FALSE;
}

void Enemy_2x1_map_move(){
    UINT8 nSprites = 2;
    // Si el enemigo no se mueve, pero hay scroll, debe ir en direccion contraria
    if(auxEnemy -> type == 's') nSprites = 4;

    
    if(!auxEnemy -> isMoving && scroll){    // Esta parte no tiene bugs
        for(j = 0; j < nSprites; j++){
            if(slime_dir == J_UP){
                scroll_sprite(auxEnemy -> sprite + j, 0, 1);
                //scroll_sprite(auxEnemy -> sprite + 1, 0, 1);
            }
            else if(slime_dir == J_DOWN){
                scroll_sprite(auxEnemy -> sprite + j, 0, -1);
                //scroll_sprite(auxEnemy -> sprite + 1, 0, -1);
            }
            else if(slime_dir == J_RIGHT){
                scroll_sprite(auxEnemy -> sprite + j, -1, 0);
                //scroll_sprite(auxEnemy -> sprite + 1, -1, 0);
            }
            else if(slime_dir == J_LEFT){
                scroll_sprite(auxEnemy -> sprite + j, 1, 0);
                //scroll_sprite(auxEnemy -> sprite + 1, 1, 0);
            }
        }
    }
    // Si hay movimiento, debe corresponder con la dirección de scroll si es que hay scroll
    else if(auxEnemy -> isMoving){
        // Mover arriba
        for(j = 0; j < nSprites; j++){
            if(auxEnemy -> dir == 0){
                if(scroll){
                    if(slime_dir == J_DOWN){
                        scroll_sprite(auxEnemy -> sprite + j, 0, -2);
                        //scroll_sprite(auxEnemy -> sprite + 1, 0, -2);
                    }
                    else if(slime_dir == J_RIGHT){
                        scroll_sprite(auxEnemy -> sprite + j, -1, -1);
                        //scroll_sprite(auxEnemy -> sprite + 1, -1, -1);
                    }
                    else if (slime_dir == J_LEFT){
                        scroll_sprite(auxEnemy -> sprite + j, 1, -1);
                        //scroll_sprite(auxEnemy -> sprite + 1, 1, -1);
                    }
                }
                else{
                    // Se mueve normal hacia arriba (sin scroll)
                    scroll_sprite(auxEnemy -> sprite + j, 0, -1);
                    //scroll_sprite(auxEnemy -> sprite + 1, 0, -1);
                }
            }
            // Mover abajo
            else if(auxEnemy -> dir == 1){
                if(scroll){
                    if(slime_dir == J_UP){
                        scroll_sprite(auxEnemy -> sprite + j, 0, 2);
                        //scroll_sprite(auxEnemy -> sprite + 1, 0, 2);
                    }   // No se queda en su lugar
                    else if(slime_dir == J_RIGHT){
                        scroll_sprite(auxEnemy -> sprite + j, -1, 1);
                        //scroll_sprite(auxEnemy -> sprite + 1, -1, 1);
                    }
                    else if(slime_dir == J_LEFT){  // LEFT
                        scroll_sprite(auxEnemy -> sprite + j, 1, 1);
                        //scroll_sprite(auxEnemy -> sprite + 1, 1, 1);
                    }
                }
                else{
                    scroll_sprite(auxEnemy -> sprite + j, 0, 1);
                    //scroll_sprite(auxEnemy -> sprite + 1, 0, 1);
                }
            }
            // Mover Derecha
            else if(auxEnemy -> dir == 2){
                if(scroll){
                    if(slime_dir == J_UP){
                        scroll_sprite(auxEnemy -> sprite + j, 1, 1);
                        //scroll_sprite(auxEnemy -> sprite + 1, 1, 1);
                        
                    }
                    else if(slime_dir == J_DOWN){
                        scroll_sprite(auxEnemy -> sprite + j, 1, -1);
                        //scroll_sprite(auxEnemy -> sprite + 1, 1, -1);
                    }
                    else if(slime_dir == J_LEFT){  // LEFT
                        scroll_sprite(auxEnemy -> sprite + j, 2, 0);
                        //scroll_sprite(auxEnemy -> sprite + 1, 2, 0);
                    }
                }
                else{
                    scroll_sprite(auxEnemy -> sprite + j, 1, 0);
                    //scroll_sprite(auxEnemy -> sprite + 1, 1, 0);
                }

            }
            else if(auxEnemy -> dir == 3){ // Izquierda
                if(scroll){
                    if(slime_dir == J_UP){
                        scroll_sprite(auxEnemy -> sprite + j, -1, 1);
                        //scroll_sprite(auxEnemy -> sprite + 1, -1, 1);
                        
                    }
                    else if(slime_dir == J_DOWN){
                        scroll_sprite(auxEnemy -> sprite + j, -1, -1);
                        //scroll_sprite(auxEnemy -> sprite + 1, -1, -1);
                    }
                    else if(slime_dir == J_RIGHT){  // LEFT
                        scroll_sprite(auxEnemy -> sprite + j, -2, 0);
                        //scroll_sprite(auxEnemy -> sprite + 1, -2, 0);
                    }
                }
                else
                    scroll_sprite(auxEnemy -> sprite + j, -1, 0);
                    //scroll_sprite(auxEnemy -> sprite + 1, -1, 0);
            }
        }
    }
    
}

void Enemy_anim_idle(){
    if(frames_anim == 30){
        UINT8 spr = auxEnemy -> sprite;
        UINT8 type = auxEnemy -> type;
        if(state){
            if(type == 'r')
                set_sprite_tile(spr, 11);
            else if (type == 's') {
                scroll_sprite(spr, 0, 1);
                scroll_sprite(spr + 2, 0, 1);
            }
        }
        else{
            if(type == 'r')
                set_sprite_tile(spr, 9);
            else if(type == 's'){
                scroll_sprite(spr, 0, -1);
                scroll_sprite(spr + 2, 0, -1);   
            }
        }
    }
}

void Enemy_anim_moving(){
    // Cantidad de sprites del enemigo
    UINT8 nSprites = 2;
    if(auxEnemy -> type == 's')
        nSprites = 4;
    
    if(frames_anim == 3){
        for(j = 0; j < nSprites; j++){   
            if(auxEnemy -> dir == 2 || auxEnemy -> dir == 3){
                if(state){
                    scroll_sprite(auxEnemy -> sprite + j, 0, -1);
                    //scroll_sprite(auxEnemy -> sprite + 1, 0, -1);
                }
                else{
                    scroll_sprite(auxEnemy -> sprite + j, 0, 1);
                    //scroll_sprite(auxEnemy -> sprite + 1, 0, 1);
                }
            }
            else{
                if(state){
                    scroll_sprite(auxEnemy -> sprite + j, -1, 0);
                    //scroll_sprite(auxEnemy -> sprite + 1, -1, 0);
                }
                else{
                    scroll_sprite(auxEnemy -> sprite + j, 1, 0);
                    //scroll_sprite(auxEnemy -> sprite + 1, 1, 0);
                }
            }
        }
    }
}

void Enemy_animMap_handler(){
    
    for(i = 0; i < nEnemies; i++){
        auxEnemy = &enemies_array[i];
        if(isMoving && auxEnemy -> isMoving)
            Enemy_anim_moving();
        else
            Enemy_anim_idle();
    }

}

void vbl_update(){
    vbl_count++;
}