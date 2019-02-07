/* 
* microBoids
* Copyright (C) 2018 Connor Brooks
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
*/

#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "boid.h"
#include "quadtree.h"
#include "utils.h"
#include "graphics.h"

// Error callback
static void error_callback(int error, const char* description)
{
  fputs(description, stderr);
}

// Keyboar callback (NOT CURRENTLY USED)
// static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
// {
//   if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//     glfwSetWindowShouldClose(window, GL_TRUE);
// }


int main(void)
{
  GLFWwindow* window;
  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);
  window = glfwCreateWindow(640, 480, "microBoids", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);
  //glfwSetKeyCallback(window, key_callback); NOT CURRENTLY USED


  // Init random number gen
  seedRandom();

  // Create the inital quadtree
  struct quadtree *thisQuadtree;
  struct vector2f qPos = {0.0f, 0.0f};
  thisQuadtree = createQuadtree(qPos, 1.0f);


  // Populate the quad with birds
  struct boid *boidFlock[NUM_BOIDS];
  for(int i = 0; i < NUM_BOIDS; i++){
    boidFlock[i] = createBoid(genRandom(), genRandom());
    quadtreeInsert(thisQuadtree, boidFlock[i]);
  }


  while (!glfwWindowShouldClose(window))
  {

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Scale to draw on 0-1 values
    glTranslatef(-1.0, -1.0, 0.0f);
    glScalef(2.0, 2.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();



    // Draw the quadtree
    drawQuadtree(thisQuadtree);

    // Update each boid
    for(int i = 0; i < NUM_BOIDS; i++){
      boidUpdate(boidFlock[i], thisQuadtree);
    }

    // free the quadtree to recreate it with updated boids
    // (hacky, yep!)
    quadtreeFree(thisQuadtree);

    // Create the new quadtree and populate it
    thisQuadtree = createQuadtree(qPos, 1.0f);
    for(int i = 0; i < NUM_BOIDS; i++){
      quadtreeInsert(thisQuadtree, boidFlock[i]);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();

  }

  // If program exits, free the quadtree and 
  // free each boid
  quadtreeFree(thisQuadtree);
  for(int i = 0; i < NUM_BOIDS; i++){
    free(boidFlock[i]);
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}

