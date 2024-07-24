// drawSegs.cpp
//
// Self-contained function to draw a set of segments.
//
// Used mainly for debugging.
//
// Segments are drawn using the most recent MVP sent to the shaders, and the currently active shaders.


#include "headers.h"
#include "drawSegs.h"


// 'nSegs' is the number of segments.
// 'segs' is an array of 2*nSegs vertices.

void Segs::drawSegs( vec3 *segs, vec3 *colours, int nSegs, mat4 &MVP )

{
  glBindVertexArray( 0 );

  GLuint VBO0, VBO1;
  
  // Set up segments
  
  glGenBuffers( 1, &VBO0 );
  glBindBuffer( GL_ARRAY_BUFFER, VBO0 );
  glBufferData( GL_ARRAY_BUFFER, nSegs * 2 * sizeof(vec3), segs, GL_STATIC_DRAW );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
  glEnableVertexAttribArray( 0 );

  // Set up colours
  
  glGenBuffers( 1, &VBO1 );
  glBindBuffer( GL_ARRAY_BUFFER, VBO1 );
  glBufferData( GL_ARRAY_BUFFER, nSegs * 2 * sizeof(vec3), colours, GL_STATIC_DRAW );
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );
  glEnableVertexAttribArray( 1 );

  // Draw

  GLint id = 0;
  glGetIntegerv(GL_CURRENT_PROGRAM, &id);
 
  gpuProg->activate();

  glUniformMatrix4fv( glGetUniformLocation( gpuProg->id(), "MVP"), 1, GL_TRUE, &MVP[0][0] );
  glDrawArrays( GL_LINES, 0, 3 );

  gpuProg->deactivate();

  glUseProgram( id );

  // Clean up
  
  glDeleteBuffers( 1, &VBO0 );
  glDeleteBuffers( 1, &VBO1 );

  glDisableVertexAttribArray( 0 );
  glDisableVertexAttribArray( 1 );

  glBindBuffer( GL_ARRAY_BUFFER, 0 );
}



void Segs::drawOneSeg( vec3 tail, vec3 head, mat4 &MVP )

{
  vec3 segs[2]    = { tail, head };
  vec3 colours[2] = { vec3(1,1,1), vec3(1,1,1) }; 

  drawSegs( segs, colours, 1, MVP );
}




// Define basic shaders


char *Segs::vertexShader =
#ifdef MACOS
  "#version 330\n"
#else
  "#version 300 es\n"
#endif
  "\n"
  "layout (location = 0) in vec4 position;\n"
  "layout (location = 1) in vec3 colour_in;\n"
  "out mediump vec3 colour;\n"
  "uniform mat4 MVP;\n"
  "\n"
  "void main()\n"
  "\n"
  "{\n"
  "  gl_Position = MVP * position;\n"
  "  colour = colour_in;\n"
  "}";



char *Segs::fragmentShader = 

#ifdef MACOS
  "#version 330\n"
#else
  "#version 300 es\n"
#endif
  "\n"
  "in mediump vec3 colour;\n"
  "out mediump vec4 fragColour;\n"
  "\n"
  "void main()\n"
  "\n"
  "{\n"
  "  fragColour = vec4( colour, 1 );\n"
  "}";


GPUProgram *Segs::setupShaders()

{
  GPUProgram *gpuProg = new GPUProgram();
  gpuProg->init( vertexShader, fragmentShader );
  return gpuProg;
}
