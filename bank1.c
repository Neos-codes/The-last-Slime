#include <gb/gb.h>
#include <rand.h>
#include "libs/chars.h"
#include "Backgrounds/testMap.c"


// Slime
// ------ Mechanics
extern void Slime_map_move();
// ------ Animations
extern void Slime_animMap_handler();
// Enemies
// ------ Mechanics
// Knight
extern void Check_2x1_collisions();
// ------ Animations
// Knight
extern void Enemy_animMap_handler();

// ----- VBlanks
extern void vbl_update();

// ----- Banks
extern UINT8 actualBank;

//================================
// ----- Global Variables ----- //
//================================
// Slime
extern UINT8 input;             // Aqui guardamos el input completo
extern UINT8 slime_dir;
extern UINT8 isMoving;          // Flag TRUE si el Slime se esta moviendo
extern UINT8 pixels_moved;      // Cantidad de pixeles (y frames) mientras el slime está en movimiento (son 16 pixeles por movimiento)
extern UINT8 frames_anim;       // Frames de animaciones (ejemplo, 30 frames = 1 cambio de sprite de animacion del Slime en idle)
extern UINT8 lastSlime_x, lastSlime_y;
extern UINT8 state;
// Utils
//----- Iterations
extern UINT8 i, j;
//----- VBlanks
extern UINT8 vbl_count;
//========================
// ----- PER STAGE -----//
//========================
// Coordenadas background
extern UINT8 bkg_x, bkg_y;


// Slime "instance" (struct)
extern struct Slime player;

// Enemies "instances" (struct)
//extern struct Enemy knight;
extern struct Enemy *auxEnemy;

extern struct Enemy enemies_array[5];
extern UINT8 nEnemies;
extern UINT8 fighting;
extern UINT8 eIndex;

//=====================================
//---------Internal Variables----------
//=====================================
UINT8 dif_x, dif_y;
UINT8 dir_x, dir_y;



//=====================================
//---------Internal Functions----------
// ----- Definiciones de funciones
void IniMap(){
    // Inicializar Animaciones
    state = TRUE;
    input = 0;
    slime_dir = 0;
    isMoving = FALSE;
    pixels_moved = 0;
    frames_anim = 0;

    // Inicializar parametros de enemigos
    for(i = 0; i < nEnemies; i++){
        enemies_array[i].isMoving = FALSE;
        //enemies_array[i].steps = 0;
        //enemies_array[i].stamina = 3;

        // Posicionar enemigos
        move_sprite(enemies_array[i].sprite, 8 + 8 * enemies_array[i].x - bkg_x, 16 + 8 * enemies_array[i].y - bkg_y);
        move_sprite(enemies_array[i].sprite + 1, 8 + 8 * enemies_array[i].x - bkg_x, 16 + 8 * (enemies_array[i].y + 1) - bkg_y);
        if(enemies_array[i].type == 's'){
            move_sprite(enemies_array[i].sprite + 2, 8 + 8 * (enemies_array[i].x + 1) - bkg_x, 16 + 8 * enemies_array[i].y - bkg_y);
            move_sprite(enemies_array[i].sprite + 3, 8 + 8 * (enemies_array[i].x + 1) - bkg_x, 16 + 8 * (enemies_array[i].y + 1) - bkg_y);
        }
    }

    // Load Map
    set_bkg_tiles(0, 0, 30, 30, testMap);

    // Set bkg position
    move_bkg(bkg_x, bkg_y);

    // Set Slime Position
    // Todas las coordenadas con "- bkg_x o y" por el desfase que hay al hacer scroll en el background
    move_sprite(0, 8 + 8 * player.x - bkg_x, 16 + 8 * (player.y + 1) - bkg_y);
    move_sprite(1, 8 + 8 * player.x - bkg_x, 16 + 8 * player.y - bkg_y);
    move_sprite(2, 8 + 8 * (player.x + 1) - bkg_x, 16 + 8 * player.y - bkg_y);
    move_sprite(3, 8 + 8 * (player.x + 1) - bkg_x, 16 + 8 * (player.y + 1) - bkg_y);
}

// Donde funciona el juego en los mapas
void MapLoop();
// Escoge una dirección estandar para los enemigos
void Enemy_Choose_dir();
// A menos de un radio "r^2" de distancia, persigue al jugador
void Enemy_Closest_dir();
// Obtener distancia del enemigo al Slime
UINT8 GetSlimeDistance();
UINT8 Check_dir_x(UINT8 *dir);
UINT8 Check_dir_y(UINT8 *dir);

// Verifica colisiones entre enemigos y Slime
void MapLoop() {

  //set_bkg_tiles(0, 0, 30, 30, testBkg);

  // Loop de juego
    while(1){

        if(!vbl_count){
            wait_vbl_done();
        }
        vbl_count = 0;

        
        // Solo se puede recibir input cuando slime está quieto
        if(!isMoving){
            input = joypad();
            // Cambiar de mapa
            if(input & J_A){
                actualBank = 2;
                // Apuntar al enemigo con el cual vamos a pelear
                auxEnemy = &enemies_array[0];
                return;
            }
        }
        
        Slime_map_move();
        
        // Animaciones del Knight
        Enemy_animMap_handler();

        // Animaciones del Slime
        Slime_animMap_handler();

        if(fighting){
            auxEnemy = &enemies_array[eIndex];
            return;
        }


        // Para frames de animacion
        frames_anim++;

    }
}

void Enemy_Choose_dir(){
    // Si aun le quedan pasos por dar en su direccion actual
    UINT8 rand_ = rand() % 4;
    if(auxEnemy -> steps != 3){
        UINT8 k;
        // Revisar si aun se puede mover en esa direccion
        for(k = 0; k < 3; k++){ 
            Check_2x1_collisions();
            // Si no se puede mover, cambiar direccion
            if(!auxEnemy -> isMoving){
                 //auxEnemy -> dir = (auxEnemy -> dir + 1) % 4;
                auxEnemy -> dir = rand_ % 4;
                auxEnemy -> steps = 0;
                rand_++;
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
        for(k = 0; k < 3; k++){
            //auxEnemy -> dir = (auxEnemy -> dir + 1) % 4;
            auxEnemy -> dir = rand_ % 4;
            Check_2x1_collisions();
            if(auxEnemy -> isMoving){
                auxEnemy -> steps = 0;
                break;
            }
            rand_++;
        }
    }
}

void Enemy_Closest_dir(){
    UINT8 k, rand_;
    k = 0;
    
    // Si enemy y slime estan desfasados en ambos ejes, escoge eje al azar para moverse
    if(dif_x && dif_y){
        rand_ = rand() % 2;

        // Moverse en X
        if(rand_){
            // Moverse a la izquierda
            if(!dir_x){
                auxEnemy -> dir = 3;
                // Asignar dir a auxEnemy
                Check_2x1_collisions();
                if(auxEnemy -> isMoving){
                    return;
                }
                else if(Check_dir_y(&auxEnemy -> dir)){
                    return;
                }
            }
            else{
                auxEnemy -> dir = 1;
                Check_2x1_collisions();
                if(auxEnemy -> isMoving){
                    return;
                }
                else if(Check_dir_y(&auxEnemy -> dir)){
                    return;
                }
            }
        }
        else{    // Moverse en Y
            if(!dir_y){
                auxEnemy -> dir = 0;
                Check_2x1_collisions();
                if(auxEnemy -> isMoving){
                    return;
                }
                else if(Check_dir_x(&auxEnemy -> dir)){
                    return;
                }
            }
            else{
                auxEnemy -> dir = 2;
                Check_2x1_collisions();
                if(auxEnemy -> isMoving){
                    return;
                }
                else if(Check_dir_x(&auxEnemy -> dir)){
                    return;
                }
            }
        }
    }

    else if(dif_x){
        // Moverse al a derecha
        if(dir_x){
            auxEnemy -> dir = 1;
            Check_2x1_collisions();
            if(auxEnemy -> isMoving){
                return;
            }
            else if(Check_dir_y(&auxEnemy -> dir)){
                return;
            }
        }
        else{  // Moverse a la izquierda
            auxEnemy -> dir = 3;
            Check_2x1_collisions();
            if(auxEnemy -> isMoving){
                return;
            }
            else if(Check_dir_y(&auxEnemy -> dir)){
                return;
            }
        }
    }
    else if(dif_y){
        // Moverse hacia abajo
        if(dir_y){
            auxEnemy -> dir = 2;
            Check_2x1_collisions();
            if(auxEnemy -> isMoving){
                return;
            }
            else if(Check_dir_x(&auxEnemy -> dir)){
                return;
            }
        }
        else{  // Moverse hacia arrba
            auxEnemy -> dir = 0;
            Check_2x1_collisions();
            if(auxEnemy -> isMoving){
                return;
            }
            else if(Check_dir_x(&auxEnemy -> dir)){
                return;
            }
        }
    }

    
    for(k = 0; k < 3; k++){
        auxEnemy -> dir = rand() % 4;
        Check_2x1_collisions();
        if(auxEnemy -> isMoving){
            auxEnemy -> steps = 0;
            return;
        }
    }

}

UINT8 Check_dir_y(UINT8 *dir){
    UINT8 rand_ = rand() % 2;
   
   if(rand_){
       *dir = 2;
       Check_2x1_collisions();
       if(auxEnemy -> isMoving){
           auxEnemy -> steps = 0;
           return TRUE;
       }
       else{
           *dir = 0;
           Check_2x1_collisions();
           if(auxEnemy -> isMoving){
               auxEnemy -> steps = 0;
               return TRUE;
           }
       }
   }
   else{
       *dir = 0;
       Check_2x1_collisions();
       if(auxEnemy -> isMoving){
           auxEnemy -> steps = 0;
           return TRUE;
       }
       else{
           *dir = 2;
           Check_2x1_collisions();
           if(auxEnemy -> isMoving){
               auxEnemy -> steps = 0;
               return TRUE;
           }
       }
   }
    
    return FALSE;
}

UINT8 Check_dir_x(UINT8 *dir){
    UINT8 rand_ = rand() % 2;
   
   if(rand_){
       *dir = 1;
       Check_2x1_collisions();
       if(auxEnemy -> isMoving){
           auxEnemy -> steps = 0;
           return TRUE;
       }
       else{
           *dir = 3;
           Check_2x1_collisions();
           if(auxEnemy -> isMoving){
               auxEnemy -> steps = 0;
               return TRUE;
           }
       }
   }
   else{
       *dir = 0;
       Check_2x1_collisions();
       if(auxEnemy -> isMoving){
           auxEnemy -> steps = 0;
           return TRUE;
       }
       else{
           *dir = 1;
           Check_2x1_collisions();
           if(auxEnemy -> isMoving){
               auxEnemy -> steps = 0;
               return TRUE;
           }
       }
   }

    return FALSE;
}

UINT8 GetSlimeDistance(){
    
    // Se puede usar lastSlime_x o lastSlime_y, pero funciona mejor con las coordenadas luego de moverse

    UINT8 x1, x2, y1, y2;
    // Si el enemigo está a la derecha
    if(auxEnemy -> x > player.x){
        x2 = auxEnemy -> x;
        x1 = player.x;
        dif_x = TRUE;
        dir_x = 0;
    }
    // Si el enemigo está a la izquierda
    else if(auxEnemy -> x < player.x){
        x2 = player.x;
        x1 = auxEnemy -> x;
        dif_x = TRUE;
        dir_x = 1;
    }
    else{
        x1 = 0;
        x2 = 0;
        dif_x = FALSE;
    }

    // Si el enemigo esta más abajo
    if(auxEnemy -> y > player.y){
        y2 = auxEnemy -> y;
        y1 = player.y;
        dif_y = TRUE;
        dir_y = 0;
    }
    else if(auxEnemy -> y < player.y){
        // Si el enemigo esta más arriba
        y2 = player.y;
        y1 = auxEnemy -> y;
        dif_y = TRUE;
        dir_y = 1;
    }
    else{
        y1 = 0;
        y2 = 0;
        dif_y = FALSE;
    }

    return (x2 - x1) + (y2 - y1);
}

UINT8 Slime_Enemy_Collisions(){
    // Si el player cae ne la misma casilla que un enemigo y el enemigo no está descansando... PELEAR!
    if(player.x == auxEnemy -> x && player.y == auxEnemy -> y && auxEnemy -> resting == 0){
        // Fight!
        return TRUE;
    }
    return FALSE;
}