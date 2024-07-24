// drawSegs.h
//
// Create a class instance:
//
//    Segs *segs = new Segs();
//
// Use it:
//
//    segs->drawOneSeg( tail, head, MVP );


#ifndef DRAW_SEGS_H
#define DRAW_SEGS_H

#include "headers.h"
#include "gpuProgram.h"


class Segs {

  static char *fragmentShader;
  static char *vertexShader;

  GPUProgram *setupShaders();

  GPUProgram *gpuProg;
  
 public:

  Segs() { 
    gpuProg = setupShaders();
  };
  
  void drawSegs( vec3 *segs, vec3 *colours, int nSegs, mat4 &MVP );
  void drawOneSeg( vec3 tail, vec3 head, mat4 &MVP );
};

#endif
