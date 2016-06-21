#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED

// Ovaa funckija treba da se povika vo main. Sodrzi povici za inicijalizacija na glut
// kako i povik kon main loop.
void initEverything(int argc, char *argv[]);

// Ovie funkcii go dvizat robotot.
void odi();
void svrti_levo();
void svrti_desno();
void zemi();
void ostavi();

#endif // INTERFACE_H_INCLUDED
