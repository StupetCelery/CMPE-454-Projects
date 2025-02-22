// ll.h

#ifndef LL_H
#define LL_H


class GPUProgram;
extern GPUProgram *myGPUProgram;

class World;
extern World *world;

typedef enum { UP, DOWN } KeyState;
extern KeyState upKey, downKey, leftKey, rightKey;

extern bool pauseGame;
extern bool showClosestPoint;
extern struct timeb startTime;

extern int screenWidth;
extern float screenAspect;

#include "drawSegs.h"
extern Segs *segs;

#endif
