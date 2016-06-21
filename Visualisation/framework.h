#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED

#include "queue.h"

// Ovaa funckija treba da se povika vo main. Sodrzi povici za inicijalizacija na glut
// kako i povik kon main loop.
void initEverything(int argc, char *argv[]);

void ANIMATE(queue * q);

#endif // INTERFACE_H_INCLUDED
