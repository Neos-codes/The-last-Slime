#include <gb/gb.h>
#include <rand.h>
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

// Estados de enemigos
extern UINT8 eAtacking;
extern UINT8 eStamina;
extern UINT16 eFrames;
extern UINT16 eTiming;
extern UINT8 atk_flag;



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

// ----- Ataques de enemigos
// Activa el ataque del enemigo, dentro de esta funcion se mueve el enemigo y hace daño
void Enemy_Attack();
// Mueve al enemigo mientras ataca, va dentro de "Enemy_Attack()"
void Enemy_Atk_Move();
// Si la bandera atk_flag es activa, verifica la posicion del slime con la zona de ataque del slime
void Collision_Atk();


//========================================
//--- Desarrollo de funciones internas ---
//========================================

// TO DO: FALTA ARREGLAR EL "RAND" para timing de enemigo
// Para mantener la racha

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

    // Inicializar parametros de enemigos en batalla
    eStamina = auxEnemy -> stamina;
    eAtacking = FALSE;     // Estado si enemigo está atacando
    eFrames = 0;           // Frames de juego que pasan para que eAtacking se active (compara con eTiming)
    eTiming = 0;           // Frames de juego que deben pasar para que eAtacking se active
    atk_flag = FALSE;      // TRUE cuando el enemigo alcanza la zona de juego del slime (verificacion de daño)
    avoiding = FALSE;
}

void BattleLoop(){

    set_bkg_tiles(0, 0, 20, 18, batteMap_test);

    IniBattle();
    
    while(1){

        if(!vbl_count)
            wait_vbl_done();
        vbl_count = 0;


        //====== ANIMACIONES ======//

        Enemy_anim_idle();
        Slime_anim_idle();

        input = joypad();

        //====== INPUT EN BATALLA ======//

        // Solo si no se está moviendo, se toman los inputs para esquivar
        if(!isMoving){
            Take_Input();
        }

        //====== TIMING DE ATAQUE PARA EL ENEMIGO ======// (El enemigo aun no ataca)

        // Escoger timing para atque de enemigo si no está atacando
        if(eTiming == 0 && !eAtacking){
            eTiming = rand() % 1000;      // Setear valor en un rango para que el enemigo ataque 
        }
        else if(!eAtacking){              // El timing corre mientras no esté atacando
            if(eFrames == eTiming){  // Si se alcanza el timing de ataque
                move_sprite(15, 8, 16);
                eAtacking = TRUE;
                eFrames = 0;         // eFrames se reutiliza para contar pixeles movidos al enemigo
                eTiming = 0;         // Reiniciar contador
            }
            else{                    // Si aun no alcanza el timing, aumentar en 1
                eFrames++;
            }
        }

        //======== ENEMIGO ATACANDO ========//
        if(eAtacking){
            Enemy_Attack();
        }


        // Si cambiamos de bank, significa que el enemigo se cansó
        if(actualBank == 1){
            return;
        }




        //====== MOVIMIENTO DEL SLIME ======//

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
        
        //====== FRAMES DE ANIMACIONES ======//
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
    if(pixels_moved < 8){
        if(slime_dir == 1){
            scroll_sprite(0, 0, -4);
            scroll_sprite(1, 0, -4);
            scroll_sprite(2, 0, -4);
            scroll_sprite(3, 0, -4);
        }
        // Esquivar hacia abajo
        else if(slime_dir == 2){
            scroll_sprite(0, 0, 4);
            scroll_sprite(1, 0, 4);
            scroll_sprite(2, 0, 4);
            scroll_sprite(3, 0, 4);
        }
        // Esquivar hacia la izquierda
        else if(slime_dir == 3){
            scroll_sprite(0, -4, 0);
            scroll_sprite(1, -4, 0);
            scroll_sprite(2, -4, 0);
            scroll_sprite(3, -4, 0);
        }
    }

    pixels_moved++;

    if(pixels_moved == 16){
        pixels_moved = 0;
        avoiding = FALSE;
    }

}

void Return_toCenter(){
    // Si estaba arriba, volver al centro
    if(slime_dir == 1){
        scroll_sprite(0, 0, 4);
        scroll_sprite(1, 0, 4);
        scroll_sprite(2, 0, 4);
        scroll_sprite(3, 0, 4);
    }
    // Si estaba abajo, volver al centro
    else if(slime_dir == 2){
        scroll_sprite(0, 0, -4);
        scroll_sprite(1, 0, -4);
        scroll_sprite(2, 0, -4);
        scroll_sprite(3, 0, -4);
    }
    // Si estaba a la izquierda, volver al centro
    else if(slime_dir == 3){
        scroll_sprite(0, 4, 0);
        scroll_sprite(1, 4, 0);
        scroll_sprite(2, 4, 0);
        scroll_sprite(3, 4, 0);
    }

    pixels_moved++;

    if(pixels_moved == 8){
        pixels_moved = 0;
        slime_dir = 0;
        isMoving = FALSE;
    }
}

void Enemy_Attack(){

    Enemy_Atk_Move();

    if(atk_flag){
        Collision_Atk();
    }
    // Verificar si está en una casilla que haga daño

    // Hacer daño

}

void Enemy_Atk_Move(){
    UINT8 eSprite;

    eSprite = auxEnemy -> sprite;

    if(eFrames < 24){    // Si enemigo se mueve hacia atrás para preparar ataque
        
        // Mover sprites base del enemigo
        scroll_sprite(eSprite, 1, 0);
        scroll_sprite(eSprite + 1, 1, 0);
        if(auxEnemy -> type == 's'){
            // Para enemigos de 4 sprites, mover los sprites faltantes
            scroll_sprite(eSprite + 2, 1, 0);
            scroll_sprite(eSprite + 3, 1, 0);
        }
    }
    else if(eFrames < 29){               // Si enemigo arremete hacia Slime
        // Mover sprites base del enemigo
        scroll_sprite(eSprite, -16, 0);
        scroll_sprite(eSprite + 1, -16, 0);
        if(auxEnemy -> type == 's'){
            // Para enemigos de 4 sprites, mover los sprites faltantes
            scroll_sprite(eSprite + 2, -16, 0);
            scroll_sprite(eSprite + 3, -16, 0);
        }

    }
    else if(eFrames < 35){   // Quedarse en posición de ataque un par de segundos
            UINT8 shake_dir;
            // Verificacion de daño en el primer frame de ataque (frame 29)
        if(eFrames == 29){
            atk_flag = TRUE;
        }
        else{
            atk_flag = FALSE;
        }

        if(eFrames & 1){   // Si eFrames es par
            shake_dir = -1;
        }
        else{
            shake_dir = 1;
        }
        
        // Mover sprites base del enemigo
        scroll_sprite(eSprite, 0, shake_dir);
        scroll_sprite(eSprite + 1, 0, shake_dir);
        if(auxEnemy -> type == 's'){
            // Para enemigos de 4 sprites, mover los sprites faltantes
            scroll_sprite(eSprite + 2, 0, shake_dir);
            scroll_sprite(eSprite + 3, 0, shake_dir);
        }

    }
    else{       // Si ya arremetió, volver a su posición central

        // Mover sprites base del enemigo
        scroll_sprite(eSprite, 8, 0);
        scroll_sprite(eSprite + 1, 8, 0);
        if(auxEnemy -> type == 's'){
            // Para enemigos de 4 sprites, mover los sprites faltantes
            scroll_sprite(eSprite + 2, 8, 0);
            scroll_sprite(eSprite + 3, 8, 0);
        }
        
        // Si el ataque termina, reiniciar parametros de ataque
        if(eFrames == 41){
            eFrames = 0;
            eAtacking = FALSE;
            eStamina--;   // Quitar 1 de estamina al enemigo
        }
    }
    // Mover al enemigo en base de frames
    eFrames++;
}

void Collision_Atk(){
    
    // Por el momento el daño es solo cuando el slime está en el centro
    if(slime_dir == 0){
        
        // Reiniciar en CheckPoint
        // TO DO CHECKPOINT
        
        // Animacion de daño
        if(player.hp > 1){
            player.hp--;
        }
        else{
            // Matar slime
            move_sprite(0, 8, 24);
            move_sprite(1, 8, 16);
            move_sprite(2, 16, 16);
            move_sprite(3, 16, 24);
        }
    }

    // Si el enemigo se queda sin stamina
    if(eStamina == 0){
        // Return to Menu
        auxEnemy -> resting = 3;
        actualBank = 1;
        return;
        
        
        // Mover sprites base del enemigo
        move_sprite(auxEnemy -> sprite, 112, 16);
        move_sprite(auxEnemy -> sprite + 1, 112, 24);
        if(auxEnemy -> type == 's'){
            // Para enemigos de 4 sprites, mover los sprites faltantes
            move_sprite(auxEnemy -> sprite + 2, 112, 16);
            move_sprite(auxEnemy -> sprite + 3, 112, 24);
        }
    }
}

