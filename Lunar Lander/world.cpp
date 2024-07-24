// world.cpp


#include "world.h"
#include "lander.h"
#include "ll.h"
#include "gpuProgram.h"
#include "strokefont.h"

#include <sstream>
#include <iomanip>

// const float textAspect = 0.7;	// text width-to-height ratio (you can use this for more realistic text on the screen)


void World::updateState( float elapsedTime )

{
  // If game is going, keep track of time
  if (timeRunning) {
    elapsedSeconds = glfwGetTime();
  }

  // See if any keys are pressed for thrust

  // If lander has lander, disable thrust

  if (!(lander->velocity.x == 0 && lander->velocity.y == 0)) {
    if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS) // right arrow
      lander->rotateCW( elapsedTime );

    if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS) // left arrow
      lander->rotateCCW( elapsedTime );

    if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS) // down arrow
      lander->addThrust( elapsedTime );
  
  }

  // Update the position and velocity

  lander->updatePose( elapsedTime );

  // See if the lander has touched the terrain

  vec3 closestTerrainPoint = landscape->findClosestPoint( lander->centrePosition() );
  float closestDistance = ( closestTerrainPoint - lander->centrePosition() ).length();

  // Find if the view should be zoomed

  zoomView = (closestDistance < ZOOM_RADIUS);

  // Check for landing or collision and let the user know
  //
  // Landing is successful if the vertical speed is less than 1 m/s and
  // the horizontal speed is less than 0.5 m/s.
  //
  // SHOULD ALSO CHECK THAT LANDING SURFACE IS HORIZONAL, BUT THIS IS
  // NOT REQUIRED IN THE ASSIGNMENT.
  //
  // SHOULD ALSO CHECK THAT THE LANDER IS VERTICAL, BUT THIS IS NOT
  // REQUIRED IN THE ASSIGNMENT.

  // YOUR CODE HERE

  // Check for landing or collision

  if (closestDistance < 5) { // Landing or Collision
  
    // Only want the first time the lander stops, as horizontal
    // and vertical velocity become 0 after landing
    if (!(lander->velocity.x == 0)) {
      
      // Record horizontal and vertical velocity when landing to be displayed on screen
      landingHorizontalSpeed = lander->velocity.x;
      landingVerticalSpeed = lander->velocity.y;

      // Calculate score
      if (abs(lander->velocity.y) < 1 && abs(lander->velocity.x) < 0.5) { //Success!
        score = 1000;
      }
      else if (abs(lander->velocity.y) < 4 && abs(lander->velocity.x) < 2) { // So close!
        score = 1000;
      }
      else if (abs(lander->velocity.y) < 10 && abs(lander->velocity.x) < 10) { // Hard landing
        score = 100;
      }
    }

    // Stop the lander
    lander->stop();

    // Stop the timer
    timeRunning = false;

  }
}



void World::draw()

{
  mat4 worldToViewTransform;

  if (!zoomView) {
    // Find the world-to-view transform that transforms the world
    // to the [-1,1]x[-1,1] viewing coordinate system, with the
    // left edge of the landscape at the left edge of the screen, and
    // the bottom of the landscape BOTTOM_SPACE above the bottom edge
    // of the screen (BOTTOM_SPACE is in viewing coordinates).

    float s = 2.0 / (landscape->maxX() - landscape->minX());

    worldToViewTransform
      = translate( -1, -1 + BOTTOM_SPACE, 0 )
      * scale( s, s, 1 )
      * translate( -landscape->minX(), -landscape->minY(), 0 );

  } else {

    // Find the world-to-view transform that is centred on the lander
    // and is 2*ZOOM_RADIUS wide (in world coordinates).

    // YOUR CODE HERE

    // Different scale factor for zoomed window
    float s = 1 / ZOOM_RADIUS;
    
    // Transform centres window on lander
    worldToViewTransform
      = scale( s, s, 1 )
      * translate( -(lander->centrePosition()).x, -(lander->centrePosition()).y, 0 );
  }

  // Draw the landscape and lander, passing in the worldToViewTransform
  // so that they can append their own transforms before passing the
  // complete transform to the vertex shader.

  landscape->draw( worldToViewTransform );
  lander->draw( worldToViewTransform );

  // Debugging: draw line between lander and closest point

  if (showClosestPoint)
    segs->drawOneSeg( landscape->findClosestPoint( lander->centrePosition() ), 
		      lander->centrePosition(), 
		      worldToViewTransform );

  // Draw the heads-up display (i.e. all text).

  // Define stringstreams for the chunk of text to
  // display on the right and left sides separately
  stringstream ssLeft;
  stringstream ssRight;

  drawStrokeString( "LUNAR LANDER", -0.2, 0.85, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP") );

  ssLeft.setf( ios::fixed, ios::floatfield );
  ssLeft.precision(1);

  ssRight.setf( ios::fixed, ios::floatfield );
  ssRight.precision(1);

  // Display score, time, and fuel on the left
  ssLeft << "SCORE " << score << "\n";
  ssLeft << "TIME " << elapsedSeconds <<"\n";
  ssLeft << "FUEL " << lander->fuel << "\n";

  // Calculate altitude
  float altitude = lander->centrePosition().y - landscape->findHeightAtX(lander->centrePosition().x);
  // Altitude can't be negative
  if (altitude < 0) {
    altitude = 0;
  }

  // Display altitude, horizontal speed, and vertical speed on the right
  ssRight << "ALTITUDE " << altitude << " m\n";

  if (lander->velocity.x > 0) { // Positive
    ssRight << "HORIZONTAL SPEED " << abs(lander->velocity.x) << " ->\n";
  }
  else { // Negative
    ssRight << "HORIZONTAL SPEED " << abs(lander->velocity.x) << " <-\n";
  }

  if (lander->velocity.y > 0) { // Positive
    ssRight << "VERTICAL SPEED " << abs(lander->velocity.y) << " ^\n";
  }
  else { // Negative
    ssRight << "VERTICAL SPEED " << abs(lander->velocity.y) << " v\n";
  }

  // Draw the strings on the window
  drawStrokeString( ssLeft.str(), -0.75, 0.75, 0.04, glGetUniformLocation( myGPUProgram->id(), "MVP") );
  drawStrokeString( ssRight.str(), 0.15, 0.75, 0.04, glGetUniformLocation( myGPUProgram->id(), "MVP") );

  // YOUR CODE HERE (modify the above code, too)

  // Define a string for the information to be posted upon game end
  stringstream ssEndStats;
  
  ssEndStats.setf( ios::fixed, ios::floatfield );
  ssEndStats.precision(1);

  // Display landing horizontal speed and landing vertical speed
  if (lander->velocity.x > 0) { // Positive
    ssEndStats << "LANDING HORIZONTAL SPEED: " << abs(landingHorizontalSpeed) << " ->\n";
  }
  else { // Negative
    ssEndStats << "LANDING HORIZONTAL SPEED: " << abs(landingHorizontalSpeed) << " <-\n";
  }
  
  if (lander->velocity.y > 0) { // Positive
    ssEndStats << "LANDING VERTICAL SPEED: " << abs(landingVerticalSpeed) << " ^\n";
  }
  else { // Negative
    ssEndStats << "LANDING VERTICAL SPEED: " << abs(landingVerticalSpeed) << " v\n";
  }


  // Check if landed
  if (lander->velocity.x == 0 && abs(lander->velocity.y) < 0.1) {
    // Check for success or failure
    // Display different messages based on landing velocity
    if (score == 1000) { // Success!
      drawStrokeString( "SUCCESSFUL LANDING", -0.4, 0.4, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP") );
    }
    else if (score == 100) { // Hard landing
      drawStrokeString( "YOU LANDED HARD\nYOU ARE HOPELESSLY MAROONED", -0.5, 0.45, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP") );
    }
    else if (score == 250) { // So close!
      drawStrokeString( "YOU HAD A ROUGH LANDING\nTHE DAMAGE APPEARS TO BE REPAIRABLE", -0.65, 0.45, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP") );
    }
    else { // Catastrophic crash
      drawStrokeString( "CATASTROPHIC CRASH", -0.4, 0.4, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP") );
    }
    // Display game end stats
    drawStrokeString( ssEndStats.str(), -0.4, 0.3, 0.04, glGetUniformLocation( myGPUProgram->id(), "MVP") );
  }
  
}
