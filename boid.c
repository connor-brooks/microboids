#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include "math.h"
#include "boid.h"
#include "quadtree.h"
#include "utils.h"
#include "graphics.h"
#define BOID_SIGHT_AREA (0.03)
#define BOID_MAX_SPEED (0.001)
#define BOID_AVOID_MAGNITUDE (0.006)


struct boid* createBoid(float x, float y){
  struct boid* temp = malloc(sizeof(struct boid));
  temp->x = x;
  temp->y = y;
  temp->xVelocity = 10*((genRandom()* 2.0) - 1.0);
  temp->yVelocity = 10*((genRandom() * 2.0) - 1.0);

  temp->sightArea = 0.1;
  return temp;
};

float boidGetX(struct boid* thisBoid){
  return thisBoid->x;
}

float boidGetY(struct boid* thisBoid){
  return thisBoid->y;
}


void boidVelocity(struct boid* thisBoid, float x, float y){
  thisBoid->xVelocity = x;
  thisBoid->yVelocity = y;
}


void boidMove(struct boid* thisBoid){

  float newX = thisBoid->x + thisBoid->xVelocity;
  float newY = thisBoid->y + thisBoid->yVelocity;

  // Teleport to the opposite side of the screen if you're gonan
  // crash
  if(newX >= 1.0){
    thisBoid->x = 0.0f;
  }

  else if( newX <= 0.0){
    thisBoid->x = 1.0f;

    // Otherwise move across
  } else {
    thisBoid->x = newX;
  }


  if(newY >= 1.0){
    thisBoid->y = 0.0f;
  }

  else if( newY <= 0.0){
    thisBoid->y = 1.0f;

    // Otherwise move across
  } else {
    thisBoid->y = newY;
  }

}

struct quadQueryReturn *getLocalBoids(struct boid* thisBoid,
    struct quadtree *thisQuadtree)
{

  // Center the viewing area around the boid
  struct vector2f centeredPoint = {
    (thisBoid->x) - (BOID_SIGHT_AREA / 2.0) ,
    (thisBoid->y) - (BOID_SIGHT_AREA / 2.0)
  };

  // Create the query container
  struct quadQueryReturn *query = quadCreateReturnArray();

  // Drawing for debug
#ifdef DEBUG_WORLD
  glColor3f(0.0, 0.0, 0.5);
  drawRectange(centeredPoint, BOID_SIGHT_AREA, 0.5);
#endif

  // Do the query
  quadQuery(query, thisQuadtree, centeredPoint,
      BOID_SIGHT_AREA);

  // Ignore self!
  for(int i = 0; i < query->boidCount; i++) {
    if(query->boids[i]->x == thisBoid->x &&
        query->boids[i]->y == thisBoid->y )
    {
      query->boids[i] = NULL;
      //      query->boidCount -= 1;
    }
  }

  // Return the query
  return query;


}

/* 
 * The following functions were wrote with help from:
 * http://www.kfish.org/boids/pseudocode.html
 * Thank you!
 * */

// Void boids too close
void boidAvoid(struct boid* thisBoid,
    struct quadQueryReturn *query)
{
  // Temporary vector
  struct vector2f directionToMove = {0.0f, 0.0f};
  // Cycle through the boids
  for(int i = 0; i < query->boidCount; i++){
    // No null boids
    struct vector2f difference = {0.0f, 0.0f};
    if(query->boids[i] != NULL){
      // Store the difference
      difference.x = query->boids[i]->x - thisBoid->x;
      difference.y = query->boids[i]->y - thisBoid->y;

      // Calculate the magnitude
      float magnitude = sqrt(
          (difference.x * difference.x) +
          (difference.y * difference.y));

      // Is the magnitude of the difference small enough?
      if(magnitude < BOID_AVOID_MAGNITUDE) {

        // Small piece of code for debugging
#ifdef DEBUG_WORLD
        struct vector2f centeredPoint = {
          (thisBoid->x) - (BOID_SIGHT_AREA/ 2.0) ,
          (thisBoid->y) - (BOID_SIGHT_AREA/ 2.0)
        };
        glColor3f(0.5, 0.0, 0.0);
        drawRectange(centeredPoint, BOID_SIGHT_AREA, 0.5);
#endif
        // combine the new direction
        directionToMove.x = directionToMove.x -
          (query->boids[i]->x - thisBoid->x);
        directionToMove.y = directionToMove.y -
          (query->boids[i]->y - thisBoid->y);
      }

    }
  }

  //Combine with boid's velocity
  thisBoid->xVelocity += directionToMove.x;
  thisBoid->yVelocity += directionToMove.y;
}


void boidAttract(struct boid* thisBoid,
    struct quadQueryReturn *query){

  struct vector2f directionToMove = {0.0f, 0.0f};
  for(int i = 0; i < query->boidCount; i++){
    // No null boids
    if(query->boids[i] != NULL){
      directionToMove.x += query->boids[i]->x;
      directionToMove.y += query->boids[i]->y;
    }
  }
  directionToMove.x = directionToMove.x / (query->boidCount-1);
  directionToMove.y = directionToMove.y / (query->boidCount-1);

  thisBoid->xVelocity += (directionToMove.x -thisBoid->x) /100;
  thisBoid->yVelocity += (directionToMove.y - thisBoid->y) / 100;
}


void boidMatch(struct boid* thisBoid,
    struct quadQueryReturn *query){

  struct vector2f velocityToMatch = {0.0f, 0.0f};
  for(int i = 0; i < query->boidCount; i++){
    // No null boids
    if(query->boids[i] != NULL){
      velocityToMatch.x += query->boids[i]->xVelocity;
      velocityToMatch.y += query->boids[i]->yVelocity;
    }
  }
  velocityToMatch.x = velocityToMatch.x / (query->boidCount-1);
  velocityToMatch.y = velocityToMatch.y / (query->boidCount-1);

  thisBoid->xVelocity += velocityToMatch.x / 5;
  thisBoid->yVelocity += velocityToMatch.y / 5;
}

void boidUpdate(struct boid* thisBoid, struct quadtree *thisQuadtree)
{
  // Create a new query to see which boids are close
  struct quadQueryReturn *query =
    getLocalBoids(thisBoid, thisQuadtree);

  // Don't bother with boid interaction updates if this boid
  // is the only one in the area
  if(query->boidCount - 1 >= 1) {

    // Boids 3 rules
    boidAvoid(thisBoid, query);
    boidAttract(thisBoid, query);
    boidMatch(thisBoid, query);

  boidLimitVelocity(thisBoid);
  }
  // Move the bird
  boidMove(thisBoid);
  quadQueryFree(query);

}


// Limit the boids' velocity
void boidLimitVelocity(struct boid* thisBoid){

  float velocityMagnitude= sqrt(
      (thisBoid->xVelocity * thisBoid->xVelocity) +
      (thisBoid->yVelocity * thisBoid->yVelocity));

  if(velocityMagnitude > BOID_MAX_SPEED){
    thisBoid->xVelocity = (thisBoid->xVelocity / velocityMagnitude)
      * BOID_MAX_SPEED;
    thisBoid->yVelocity = (thisBoid->yVelocity / velocityMagnitude)
      * BOID_MAX_SPEED;
  }

}
