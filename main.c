// Utils
#include <gb/gb.h>
#include "libs/chars.h"
//#include <stdio.h>
#include <rand.h>
// Sprites
#include "Sprites/slime.c"
#include "Sprites/enemies.c"
// Background tiles
#include "Background_Tiles/test_bkg.c"
// Backgrounds
//#include "Backgrounds/testMap.c"

//============================
// ----- Functions ----- //
//===========================
// GameController
void IniGame();


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
// Las animaciones de los enemigos deben ir antes que la del Slime
// Knight
void Enemy_anim_idle();
void Enemy_anim_moving();
// Escoge cual animación hacer segun la accion del enemigo
void Enemy_animMap_handler();
// Utils
// ----- RANDOM NUMBERS
void Set_seed_rand();
// -----
// Esconde los sprites de los enemigos al comenzar una batalla
void HideSprites();
// ----- VBlanks
void vbl_update();

// External functions
// ----- Map Game
extern unsigned char testMap [];
extern void MapLoop();
extern void Enemy_Choose_dir();
extern void Enemy_Closest_dir();
extern UINT8 GetSlimeDistance();
extern UINT8 Slime_Enemy_Collisions();
extern UINT8 IniMap();

// ----- Battle Game
extern void BattleLoop();

UINT8 distance;

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
UINT8 lastSlime_x, lastSlime_y;
// Background
UINT16 bkg_x, bkg_y;
UINT8 scroll;
// Utils
// Iteration
UINT8 i, j;
//----- Random
UINT16 seed;
//----- VBlanks
UINT8 vbl_count;
//----- Banks
UINT8 actualBank;
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

//==========================
//------IN BATTLE VARS------
//==========================
// Estados del Slime
UINT8 avoiding;
UINT8 fighting;
UINT8 isDead;
// Estados de enemigos
UINT8 eAtacking;
UINT16 eFrames;
UINT16 eTiming;
UINT8 eStamina;
UINT8 atk_flag;
// Utils para la batalla
UINT8 eIndex;    // Guarda el indice del enemigo


void main(){

    
    
    IniGame();

    // Para fluidez
    disable_interrupts();
    add_VBL(vbl_update);
    add_VBL(Set_seed_rand);
    set_interrupts(VBL_IFLAG);
    enable_interrupts();

    SHOW_SPRITES;
    SHOW_BKG;
    DISPLAY_ON;

    while(1){
        if(actualBank == 1){
            SWITCH_ROM_MBC1(1);
            vbl_count = 0;
            fighting = FALSE;
            IniMap();
            MapLoop();
        }
        else if(actualBank == 2){
            SWITCH_ROM_MBC1(2);
            HideSprites();
            BattleLoop();
        }
    }
}

void IniGame(){
    // Inicializar utils
    vbl_count = 0;
    distance = 0;

    // Battle vars
    eIndex = 0;          // Index enemigo a pelear
    avoiding = FALSE;    // Bool estado cuando slime está esquivando
    fighting = FALSE;    // Bool estado activa cuando slime está en una batalla


    // Inicializar banks
    actualBank = 1;


    // Inicializar Animaciones
    state = TRUE;
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
    player.hp = 3;
    player.x = 1;   // para pruebas
    player.y = 1;
    lastSlime_x = 1;
    lastSlime_y = 1;
    isDead = FALSE;

    // Inicializar parametros de enemigos
    nEnemies = 2;
    // Test knight
    enemies_array[0].x = 5;
    enemies_array[0].y = 5;
    //enemies_array[0].dir = rand() % 4;
    enemies_array[0].sprite = 4;
    enemies_array[0].isMoving = FALSE;
    enemies_array[0].type = 'r';
    enemies_array[0].steps = 0;
    enemies_array[0].stamina = 3;
    enemies_array[0].resting = 0;
    // Test Skeleton
    enemies_array[1].x = 3;
    enemies_array[1].y = 7;
    enemies_array[1].sprite = 6;
    enemies_array[1].isMoving = FALSE;
    enemies_array[1].type = 's';
    enemies_array[1].steps = 0;
    enemies_array[1].stamina = 3;
    enemies_array[1].resting = 0;
    //enemies_array[0].dir = rand() % 4;


    // Inicializar parametros de RANDOM
    seed = 0;

    //==========================
    // Set background tiles
    //==========================
    set_bkg_data(0, 4, test_bkg);
    set_bkg_tiles(0, 0, 30, 30, testMap);

    //==========================
    // Set Slime Sprites
    //==========================
    set_sprite_data(0, 13, slime);
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
    set_sprite_data(13, 7, Enemies);
    // Knight
    set_sprite_tile(4, 13);
    set_sprite_tile(5, 14);
    move_sprite(4, 8 + 8 * enemies_array[0].x, 16 + 8 * enemies_array[0].y);
    move_sprite(5, 8 + 8 * enemies_array[0].x, 16 + 8 * (enemies_array[0].y + 1));
    // Skeleton
    set_sprite_tile(6, 16);
    set_sprite_tile(7, 17);
    set_sprite_tile(8, 18);
    set_sprite_tile(9, 19);
    move_sprite(6, 8 + 8 * enemies_array[1].x, 16 + 8 * enemies_array[1].y + 1);   // .y + 1 por animacion idle
    move_sprite(7, 8 + 8 * enemies_array[1].x, 16 + 8 * (enemies_array[1].y + 1));
    move_sprite(8, 8 + 8 * (enemies_array[1].x + 1), 16 + 8 * enemies_array[1].y + 1);  // .y + 1 por anim idle
    move_sprite(9, 8 + 8 * (enemies_array[1].x + 1), 16 + 8 * (enemies_array[1].y + 1));

}

void Check_scroll_bkg(){
    // 160 debe ser width_map - 80
    // dir = RIGHT &&  MITAD DE PANTALLA  && MEDIA PANTALLA ANTES DEL BORDE DERECHO DEL MAPA
    
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
    UINT8 k;
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
            
            // Si se activa bandera de movimiento, los enemigos escogen direccion para moverse
            if(isMoving){
                for(i = 0; i < nEnemies; i++){
                    auxEnemy = &enemies_array[i];

                    // Si el enemigo tiene Stamina
                    if(auxEnemy -> resting == 0){
                        //Check_2x1_collisions();
                        if(GetSlimeDistance() < 21){
                            Enemy_Closest_dir();
                        }
                        else{
                            Enemy_Choose_dir();
                        }
                    }
                    // Si no tiene stamina, disminuir en 1 la cantidad de turnos que debe descansar
                    else{
                        auxEnemy -> resting--;
                    }
                }
                frames_anim = 0;
            }
        }
    }
    
    // Si se está moviendo, mueve a los enemigos y 
    // termina de mover 8 pixeles con la ultima direccion obtenida
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
            // Revisar colisiones con enemigos
            for(k = 0; k < nEnemies; k++){
                auxEnemy = &enemies_array[k];
                if(Slime_Enemy_Collisions()){
                    //move_sprite(15, 8 + k * 8, 16);
                    actualBank = 2;
                    eIndex = k;
                    fighting = TRUE;
                    return;
                }
            }
        }
        // Una vez se mueve los 16 pixeles, se puede recibir input de movimiento nuevamente

    }
}

void Slime_map_collision(){
    lastSlime_y = player.y;
    lastSlime_x = player.x;
    
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

// Esconde los sprites al cambiar de bank
void HideSprites(){
    UINT8 k, sprite;
    for(k = 0; k < nEnemies; k++){
        sprite = enemies_array[k].sprite;
        if((struct Enemy *)&enemies_array[k] != (struct Enemy *)auxEnemy){
            move_sprite(sprite, 0, 0);
            move_sprite(sprite + 1, 0, 0);
            if(enemies_array[k].type == 's'){
                move_sprite(sprite + 2, 0, 0);
                move_sprite(sprite + 3, 0, 0);
            }
        }
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
            return;
        }
        else{
            auxEnemy -> isMoving = FALSE;
            return;
        }
    }
    else if(auxEnemy -> dir == 2){ // DOWN
        if(testMap[auxEnemy -> x + 30 * (auxEnemy -> y + 2)] != 0x03){  // BIEN
            auxEnemy -> isMoving = TRUE;
            auxEnemy -> y += 2;
        }
        else{
            auxEnemy -> isMoving = FALSE;
            return;
        }
    }
    else if(auxEnemy -> dir == 1){ // RIGHT
        if(testMap[auxEnemy -> x + 2 + 30 * auxEnemy -> y] != 0x03){
            auxEnemy -> isMoving = TRUE;
            auxEnemy -> x += 2;
        }
        else{
            auxEnemy -> isMoving = FALSE;
            return;
        }
    }
    else if(auxEnemy -> dir == 3){ // LEFT
        if(testMap[auxEnemy -> x - 1 + 30 * auxEnemy -> y] != 0x03){
            auxEnemy -> isMoving = TRUE;
            auxEnemy -> x -= 2;
        }
        else{
            auxEnemy -> isMoving = FALSE;
            return;
        }
    }
    //knight.isMoving = FALSE;
}

void Enemy_2x1_map_move(){
    UINT8 nSprites = 2;
    // Si el enemigo no se mueve, pero hay scroll, debe ir en direccion contraria
    if(auxEnemy -> type == 's') nSprites = 4;

    
    // Si el enemigo no se mueve, debe considerar el scroll
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
            else if(auxEnemy -> dir == 2){
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
            else if(auxEnemy -> dir == 1){
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
                set_sprite_tile(spr, 15);
            else if (type == 's') {
                scroll_sprite(spr, 0, 1);
                scroll_sprite(spr + 2, 0, 1);
            }
        }
        else{
            if(type == 'r')
                set_sprite_tile(spr, 13);
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
            if(auxEnemy -> dir == 1 || auxEnemy -> dir == 3){
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