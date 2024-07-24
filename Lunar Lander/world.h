// world.h


#ifndef WORLD_H
#define WORLD_H


#include "headers.h"
#include "landscape.h"
#include "lander.h"
#include "ll.h"


#define BOTTOM_SPACE 0.1f // amount of blank space below terrain (in viewing coordinates) 


class World {

  Landscape *landscape;
  Lander    *lander;
  bool       zoomView; // show zoomed view when lander is close to landscape
  GLFWwindow *window;
  char *showMsg;
  int  score;

  // Record horizontal and vertical velocity when landing to be displayed on screen
  float landingHorizontalSpeed, landingVerticalSpeed;

  // Keep track of elapsed time
  float elapsedSeconds = 0;
  bool timeRunning = true;


  struct timeb prevTime, thisTime;

 public:

  World( GLFWwindow *w ) {
    landscape = new Landscape();
    lander    = new Lander( maxX(), maxY() ); // provide world size to help position lander
    zoomView  = false;
    window    = w;
    showMsg   = NULL;
    score     = 0;
    glfwSetTime(0);
  }

  void draw();

  void updateState( float elapsedTime );

  void resetLander() {
    lander->reset();
    score = 0;
    timeRunning = true;
    glfwSetTime(0);
  }

  // World extremes (in world coordinates)

  float minX() { return 0; }
  float maxX() { return landscape->maxX(); }

  float minY() { return 0; }
  float maxY() { return (landscape->maxX() - landscape->minX()) / screenAspect * (2 - BOTTOM_SPACE) / 2; }
};


#endif
