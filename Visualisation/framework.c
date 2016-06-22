#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "framework.h"

typedef struct Image
{
    unsigned long sizeX;
    unsigned long sizeY;
    char* data;
} Image;

typedef struct Point
{
    GLdouble x;
    GLdouble y;
} Point;

typedef struct Indices
{
    GLint i;
    GLint j;
} Indices;
int equal(Indices indices1, Indices indices2)
{
    return (indices1.i == indices2.i && indices1.j == indices2.j);
}

// ===========================================================================
// Data
// ===========================================================================

queue* moves;

static GLuint robot_image;
static GLuint coin_image;
static GLuint tile_image;
static GLuint barrier_hotizontal_image;
static GLuint barrier_vertical_image;

// This variable represents the quad dimension.
const GLdouble d = 1.0;

const GLint fields = 10;
Point grid[10][10];

const GLint coins_count = 4;
Indices coin_positions[4] = { {6, 3}, {2, 8}, {8, 7}, {4, 4} };
const Indices taken_coin = {-1, -1};

Indices robot_position = {2, 4};

// This variable holds the number of taken_coin coins by the robot.
GLint taken_coins = 0;

// north = 0
// east  = 1
// south = 2
// west  = 3
GLint robot_direction = 0;

// ===========================================================================
// End Data
// ===========================================================================

// ===========================================================================
// Function signatures
// ===========================================================================

void init();
void init_grid();

void draw_quad(GLdouble quad_size);
void draw_tiles();
void draw_coins();
// mode  : row based (0) or column based (1) drawing
// index : the row or column
// start : starting index
// end   : ending index
void draw_barriers(GLint mode, GLint index, GLint start, GLint end);
void draw_robot();

void odi();         // 0
void svrti_levo();  // 1
void svrti_desno(); // 2
void zemi();        // 3
void ostavi();      // 4

void LoadTexture(char* fname, GLuint* texName);
int ImageLoad(char* filename, Image* image);

// ===========================================================================
// End Function signatures
// ===========================================================================

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);

    draw_tiles();

    // frame
    draw_barriers(0, 0,          0, fields - 1);
    draw_barriers(0, fields - 1, 0, fields - 1);
    draw_barriers(1, 0,          1, fields - 2);
    draw_barriers(1, fields - 1, 1, fields - 2);

    draw_coins();
    draw_barriers(0, 2, 2, 6);
    draw_barriers(1, 6, 4, 7);
    draw_robot();

    glutSwapBuffers();
}

void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'w':
            odi();
            break;
        case 'a':
            svrti_levo();
            break;
        case 'd':
            svrti_desno();
            break;
        case 'z':
            zemi();
            break;
        case 'x':
            ostavi();
            break;
    }

    glutPostRedisplay();
}

void idle(void)
{
    while(!empty(moves))
    {
        int move = dequeue(moves);
        switch (move)
        {
        case 0: { odi();         Sleep(500); display(); printf("odi\n");         break; }
        case 1: { svrti_levo();  Sleep(0);   display(); printf("svrti_levo\n");  break; }
        case 2: { svrti_desno(); Sleep(0);   display(); printf("svrti_desno\n"); break; }
        case 3: { zemi();        Sleep(0);   display(); printf("zemi\n");        break; }
        case 4: { ostavi();      Sleep(0);   display(); printf("ostavi\n");      break; }
        }
    }

}

void initEverything(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(10, 10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("ROBOL");

    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    init();

    glutMainLoop();
}

void ANIMATE(queue * q)
{
    moves = q;
}

// ===========================================================================
// Init functions
// ===========================================================================

void init()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-fields * 0.5, fields * 0.5, -fields * 0.5, fields * 0.5);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_TEXTURE_2D);

    init_grid();

    LoadTexture("graphics\\robot.bmp", &robot_image);
    LoadTexture("graphics\\coin.bmp", &coin_image);
    LoadTexture("graphics\\tile.bmp", &tile_image);
    LoadTexture("graphics\\barrier_h.bmp", &barrier_hotizontal_image);
    LoadTexture("graphics\\barrier_v.bmp", &barrier_vertical_image);
}

void init_grid()
{
    GLdouble vertical_count_half = fields / 2 - d/2;
    GLdouble horizontal_count_half = fields / 2 - d/2;

    GLint horizontal = 0;
    GLdouble i, j;
    for(j = -horizontal_count_half * d; j <= horizontal_count_half * d; j += d)
    {
        GLint vertical = 0;
        for(i = - vertical_count_half * d; i <= vertical_count_half * d; i += d)
        {
            grid[vertical][horizontal].x = i;
            grid[vertical][horizontal].y = j;
            vertical++;
        }
        horizontal++;
    }
}

// ===========================================================================
// End Init functions
// ===========================================================================

// ===========================================================================
// Drawing functions
// ===========================================================================

void draw_quad(GLdouble quad_size)
{
    quad_size = quad_size / 2;
    glBegin(GL_TRIANGLES);
        glTexCoord2d(0.0, 0.0);
        glVertex4d(-quad_size, -quad_size, 0.0, 1.0);
        glTexCoord2d(1.0, 0.0);
        glVertex4d( quad_size, -quad_size, 0.0, 1.0);
        glTexCoord2d(1.0, 1.0);
        glVertex4d( quad_size,  quad_size, 0.0, 1.0);
        glTexCoord2d(0.0, 0.0);
        glVertex4d(-quad_size, -quad_size, 0.0, 1.0);
        glTexCoord2d(1.0, 1.0);
        glVertex4d( quad_size,  quad_size, 0.0, 1.0);
        glTexCoord2d(0.0, 1.0);
        glVertex4d(-quad_size,  quad_size, 0.0, 1.0);
    glEnd();
}

void draw_tiles()
{
    glBindTexture(GL_TEXTURE_2D, tile_image);
    GLint i, j;
    for(j = 0; j < fields; ++j)
    {
        for(i = 0; i < fields; ++i)
        {
            glPushMatrix();
                glTranslated(grid[i][j].x, grid[i][j].y, 0.0);
                draw_quad(d);
            glPopMatrix();
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_coins()
{
    glBindTexture(GL_TEXTURE_2D, coin_image);
    GLint i;
    for(i = 0; i < coins_count; ++i)
    {
        if(equal(coin_positions[i], taken_coin))
            continue;
        GLint ci = coin_positions[i].i;
        GLint cj = coin_positions[i].j;
        glPushMatrix();
            glTranslated(grid[ci][cj].x, grid[ci][cj].y, 0.0);
            draw_quad(d);
        glPopMatrix();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_barriers(GLint mode, GLint index, GLint start, GLint end)
{
    GLint i;
    if(mode == 0)
    {
        glBindTexture(GL_TEXTURE_2D, barrier_vertical_image);
        for(i = start; i <= end; ++i)
        {
            glPushMatrix();
                glTranslated(grid[i][index].x, grid[i][index].y, 0.0);
                draw_quad(d);
            glPopMatrix();
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    if(mode == 1)
    {
        glBindTexture(GL_TEXTURE_2D, barrier_hotizontal_image);
        for(i = start; i <= end; ++i)
        {
            glPushMatrix();
                glTranslated(grid[index][i].x, grid[index][i].y, 0.0);
                draw_quad(d);
            glPopMatrix();
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void draw_robot()
{
    glBindTexture(GL_TEXTURE_2D, robot_image);
    GLint ri = robot_position.i;
    GLint rj = robot_position.j;
    glPushMatrix();
        glTranslated(grid[ri][rj].x, grid[ri][rj].y , 0.0);
        draw_quad(d);
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
}

// ===========================================================================
// End Drawing functions
// ===========================================================================

// ===========================================================================
// Robot movement
// ===========================================================================

void odi()
{
    switch (robot_direction)
    {
        case 0: // north
            robot_position.j =
               robot_position.j + 1 >= fields ? robot_position.j : robot_position.j + 1;
            break;
        case 1: // east
            robot_position.i =
               robot_position.i + 1 >= fields ? robot_position.i : robot_position.i + 1;
            break;
        case 2: // south
            robot_position.j =
               robot_position.j - 1 < 0 ? robot_position.j : robot_position.j - 1;
            break;
        case 3: // west
            robot_position.i =
               robot_position.i - 1 < 0 ? robot_position.i : robot_position.i - 1;
            break;
    }
}

void svrti_levo()
{
    robot_direction -= 1;
    robot_direction < 0 ? robot_direction = 3 : 0;
}

void svrti_desno()
{
    robot_direction += 1;
    robot_direction > 3 ? robot_direction = 0 : 0;
}

void zemi()
{
    GLint i;
    for(i = 0; i < coins_count; ++i)
    {
        if(equal(robot_position, coin_positions[i]))
        {
            coin_positions[i] = taken_coin;
            taken_coins++;
            break;
        }
    }
}

void ostavi()
{
    if(taken_coins > 0)
    {
        GLint i;
        for(i = 0; i < coins_count; ++i)
        {
            if(equal(coin_positions[i], taken_coin))
            {
                coin_positions[i] = robot_position;
                taken_coins--;
                break;
            }
        }
    }
}

// ===========================================================================
// End Robot movement
// ===========================================================================

// ===========================================================================
// Utils functions
// ===========================================================================

void LoadTexture(char* fname, GLuint* texName)
{
    Image *img;

    img = (Image *) malloc(sizeof(Image));
    if (img == NULL)
    {
        printf("Error allocating space for image");
        exit(0);
    }

    if (!ImageLoad(fname, img))
    {
        exit(1);
    }

    glGenTextures(1, texName);
    glBindTexture(GL_TEXTURE_2D, *texName);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, img->sizeX, img->sizeY,
                 0, GL_RGB, GL_UNSIGNED_BYTE, img->data);
}

int ImageLoad(char *filename, Image *image) {
    FILE *file;
    unsigned long size;                 // size of the image in bytes.
    unsigned long i;                    // standard counter.
    unsigned short int planes;          // number of planes in image (must be 1)
    unsigned short int bpp;             // number of bits per pixel (must be 24)
    char temp;                          // temporary color storage for bgr-rgb conversion.
    if ((file = fopen(filename, "rb"))==NULL)    {
    	printf("File Not Found : %s\n",filename);
	    return 0;
    }
    fseek(file, 18, SEEK_CUR);
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
     	printf("Error reading width from %s.\n", filename);
	    return 0;
    }
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
	    printf("Error reading height from %s.\n", filename);
	    return 0;
    }
    size = image->sizeX * image->sizeY * 3;
    if ((fread(&planes, 2, 1, file)) != 1) {
	    printf("Error reading planes from %s.\n", filename);
	    return 0;
    }
    if (planes != 1) {
	     printf("Planes from %s is not 1: %u\n", filename, planes);
	     return 0;
    }
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
	     printf("Error reading bpp from %s.\n", filename);
	     return 0;
    }
    if (bpp != 24) {
	      printf("Bpp from %s is not 24: %u\n", filename, bpp);
	      return 0;
    }
    fseek(file, 24, SEEK_CUR);
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
	      printf("Error allocating memory for color-corrected image data");
      	return 0;
    }
    if ((i = fread(image->data, size, 1, file)) != 1) {
	     printf("Error reading image data from %s.\n", filename);
	     return 0;
    }
    for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
      	temp = image->data[i];
	      image->data[i] = image->data[i+2];
	      image->data[i+2] = temp;
    }
    return 1;
}
// ===========================================================================
// End Utils functions
// ===========================================================================
