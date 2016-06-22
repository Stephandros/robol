#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED

#include <GL/glut.h>
#include "queue.h"

typedef struct Indices
{
    GLint i;
    GLint j;
} Indices;

typedef struct Barrier_Data
{
    GLint mode;
    GLint index;
    GLint start;
    GLint end;
} Barrier_Data;

// Ovaa funckija treba da se povika vo main. Sodrzi povici za inicijalizacija na glut
// kako i povik kon main loop.
void initEverything(int argc, char *argv[]);

void ANIMATE(queue * q);

void set_grid(GLint f);
void set_robot_position(GLint i, GLint j);
void set_robot_direction(GLint direction);
void set_coin_positions(Indices positions[], int count);
void set_barrier_positions(Barrier_Data positions[], int count);

#endif // INTERFACE_H_INCLUDED
