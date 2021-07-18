#ifndef STRUCTS_H
#define STRUCTS_H

#include <gb/gb.h>


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
    UINT8 dir;            // 0 Up  1 Down  2 Right  3 Left
    UINT8 steps;
};

#endif