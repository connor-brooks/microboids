#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "boid.h"
#include "quadtree.h"
#include "utils.h"
#include "graphics.h"
#define GRAPHICS_BOIDS_SIZE (1.0f)


void drawBoid(struct boid *thisBoid){
  glPointSize(1.0);
  glBegin(GL_POINTS);
  glVertex3f(thisBoid->x, thisBoid->y, -0.01);
  glEnd();
}


void drawQuadtree(struct quadtree* thisQuad){

  // Draw the quad
  glColor3f(0.1f, 0.1f, 0.1f);
  drawRectange(thisQuad->position, thisQuad->size, 0.05f);

  // For each boid in the quads boid array
  for(int i = 0; i < QUADTREE_MAX_BOID_PER_CELL; i++){
    // Make sure the pointer isn't null
    if(thisQuad->boids[i] != NULL){
      // and draw the boid
      glColor3f(1.0f, 1.0f, 1.0f);
      drawBoid(thisQuad->boids[i]);
    }
  }

  // Recursivly draw all the other quadtrees
  if(thisQuad->hasSplit){
    drawQuadtree(thisQuad->topLeft);
    drawQuadtree(thisQuad->topRight);
    drawQuadtree(thisQuad->bottomLeft);
    drawQuadtree(thisQuad->bottomRight);
  }
}


void drawRectange(struct vector2f position, float size, float width){

  // Generate the points (should generate these when quad
  // subdivides, so only done once. Move this later
  struct vector2f topLeft = {position.x, position.y};
  struct vector2f topRight = {position.x + size, position.y};
  struct vector2f bottomLeft = {position.x, position.y + size};
  struct vector2f bottomRight = {position.x + size,
    position.y + size};


  glLineWidth(width);
  glBegin(GL_LINES);

  glVertex3f(topLeft.x, topLeft.y, -0.01f);
  glVertex3f(topRight.x, topRight.y, -0.01f);

  glVertex3f(topRight.x, topRight.y, -0.01f);
  glVertex3f(bottomRight.x, bottomRight.y, -0.01f);

  glVertex3f(bottomRight.x, bottomRight.y, -0.01f);
  glVertex3f(bottomLeft.x, bottomLeft.y, -0.01f);

  glVertex3f(bottomLeft.x, bottomLeft.y, -0.01f);
  glVertex3f(topLeft.x, topLeft.y, -0.01f);

  glEnd();

}

