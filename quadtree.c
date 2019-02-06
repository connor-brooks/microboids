#include <stdio.h>
#include <stdlib.h>
#include "boid.h"
#include "quadtree.h"
#include "utils.h"
#include "GL/gl.h"


struct quadtree* createQuadtree(struct vector2f position,
    float size)
{
  // Allocate memory for this quad
  struct quadtree* temp = malloc(sizeof(struct quadtree));

  // set all pointers to sub quds to null
  temp->topLeft = NULL;
  temp->topRight = NULL;
  temp->bottomLeft = NULL;
  temp->bottomRight = NULL;

  // null out all the boid pointers for this segment
  for(int i = 0; i < QUADTREE_MAX_BOID_PER_CELL; i++){
    temp->boids[i] = NULL;
  }

  // Store the location positions
  temp->position = position;
  temp->size = size;

  // Set the boidCount
  temp->boidCount = 0;

  // By default, quad not split
  temp->hasSplit = 0;
  return temp;
}

// Inserts a boid into the quadtree,
// takes the quadtree in question and the boid to be inserted,
// both as pointers as arguments
void quadtreeInsert(struct quadtree *thisQuad,
    struct boid *thisBoid)
{
  int arraySearchPos;

  // All below is sub section positions for in case the tree splits, however it
  // should soon be moved to it's own function, as this sub splitting
  // doesn't always occur
  float subQuadSize = thisQuad->size / 2.0f;

  struct vector2f topLeftSubPos = thisQuad->position;

  struct vector2f topRightSubPos = {
    thisQuad->position.x + subQuadSize,
    thisQuad->position.y
  };

  struct vector2f bottomLeftSubPos = {
    thisQuad->position.x,
    thisQuad->position.y + subQuadSize
  };

  struct vector2f bottomRightSubPos = {
    thisQuad->position.x + subQuadSize,
    thisQuad->position.y + subQuadSize
  };

  // Ignore nulll pointers to boids
  if(thisBoid == NULL) { printf("NULL BOID\n"); return; }

  // Ignore if outside of this quad
  if(!(
        (thisBoid->x >= thisQuad->position.x) &&
        (thisBoid->x <= thisQuad->position.x + thisQuad->size ) &&
        (thisBoid->y >= thisQuad->position.y) &&
        (thisBoid->y <= thisQuad->position.y + thisQuad->size)
      ))
  {
    return;
  }
  // If the quad is full, begin the quad split, this should probably go
  // into it's own function also
  if(thisQuad->boidCount == QUADTREE_MAX_BOID_PER_CELL)
  {

    // Create the new sub quads if we haven't split before
    if(!thisQuad->hasSplit){
      //Create new sub quads
      thisQuad->topLeft = createQuadtree(topLeftSubPos,
          subQuadSize);

      thisQuad->topRight = createQuadtree(topRightSubPos,
          subQuadSize);

      thisQuad->bottomLeft = createQuadtree(bottomLeftSubPos,
          subQuadSize);

      thisQuad->bottomRight = createQuadtree(bottomRightSubPos,
          subQuadSize);

      thisQuad->hasSplit = 1;
    }
    // And insert the item into whichever subquad
    quadtreeInsert(thisQuad->topLeft, thisBoid);
    quadtreeInsert(thisQuad->topRight, thisBoid);
    quadtreeInsert(thisQuad->bottomLeft, thisBoid);
    quadtreeInsert(thisQuad->bottomRight, thisBoid);

    // If this quad isn't full
  } else {

    // Loop to find empty array element
    arraySearchPos = 0;
    while (thisQuad->boids[arraySearchPos] != NULL)
      arraySearchPos++;

    // Insert the boid
    thisQuad->boids[arraySearchPos] = thisBoid;

    // Increase the boidCount
    thisQuad->boidCount += 1;
  }

}

// Function to run update on all the boids, takes the quadtree
// in question as the argumet, works recursivley
void quadtreeUpdate(struct quadtree *thisQuad){
  // Loop through each of the boids in the current quad, ignoring
  // any null pointer
  for(int i = 0; i < QUADTREE_MAX_BOID_PER_CELL; i++){
    if(thisQuad->boids[i] != NULL){
      boidMove(thisQuad->boids[i]);
    }
  }
  // Now begin recursion
  if(thisQuad->hasSplit){
    quadtreeUpdate(thisQuad->topLeft);
    quadtreeUpdate(thisQuad->topRight);
    quadtreeUpdate(thisQuad->bottomLeft);
    quadtreeUpdate(thisQuad->bottomRight);
  }
}


int quadIntersects(struct quadtree *thisQuad,
    struct vector2f position, float size)
{
  if ((position.x < thisQuad->position.x + thisQuad->size) &&
      (position.y < thisQuad->position.y + thisQuad->size) &&
      (thisQuad->position.x< position.x + size) &&
      (thisQuad->position.y < position.y + size))
  {
    return 1;
  } else { return 0; }
}


// Create a datastrucure for returning any query results
// from the quadtree
struct quadQueryReturn *quadCreateReturnArray(void)
{
  struct quadQueryReturn *temp;
  temp = malloc(sizeof(struct quadQueryReturn));
  temp->arrayCapacity = 16;
  temp->boidCount = 0;
  temp->boids = malloc(temp->arrayCapacity * sizeof(struct boid*));

  return temp;
}


void quadQueryAddBoids(struct quadQueryReturn *thisReturn,
    struct boid *boids[], struct vector2f queryPosition,
    float querySize)
{
  // Check if the array is big enough
  // Grow the array if not
  //
  if(thisReturn->boidCount + QUADTREE_MAX_BOID_PER_CELL
      >= thisReturn->arrayCapacity){

    // Double the array capacity
    thisReturn->arrayCapacity *= 2;

    // Realloc some space
    thisReturn->boids = realloc(thisReturn->boids,
        thisReturn->arrayCapacity * sizeof(struct boid*));
  }

  // Loop through each quad's boid array, adding the
  // non null pointers
  for(int i = 0; i < QUADTREE_MAX_BOID_PER_CELL; i++) {
    // if pointer isn't null
    if(boids[i] != NULL) {

      // and if the boid is within the query's boundaries
      if(boids[i]->x > queryPosition.x &&
          boids[i]->x < queryPosition.x + querySize &&
          boids[i]->y > queryPosition.y &&
          boids[i]->y < queryPosition.y + querySize) {

        // add the bird
        thisReturn->boids[thisReturn->boidCount] = boids[i];
        // Increase the bird count
        thisReturn->boidCount++;
      }

    }
  }

}


void quadQuery(struct quadQueryReturn *queryReturn,
    struct quadtree *thisQuad, struct vector2f queryPosition,
    float querySize)
{
  // Check to see if there is an intercept
  if(!quadIntersects(thisQuad, queryPosition, querySize)) {
    return;
  }
  // Recusiv go through the quadtree
  if(thisQuad->hasSplit) {
    quadQuery(queryReturn, thisQuad->topLeft, queryPosition,
        querySize);
    quadQuery(queryReturn, thisQuad->topRight, queryPosition,
        querySize);
    quadQuery(queryReturn, thisQuad->bottomLeft, queryPosition,
        querySize);
    quadQuery(queryReturn, thisQuad->bottomRight, queryPosition,
        querySize);
  }
  // Add any boids into the array
  //
  quadQueryAddBoids(queryReturn, thisQuad->boids, queryPosition,
      querySize);
}


// Recursivivley free the quadtree
void quadtreeFree(struct quadtree *thisQuad){
  if(thisQuad->hasSplit){
    quadtreeFree(thisQuad->topLeft);
    quadtreeFree(thisQuad->topRight);
    quadtreeFree(thisQuad->bottomLeft);
    quadtreeFree(thisQuad->bottomRight);
  }
  free(thisQuad);
}


// Free quad query result structure
void quadQueryFree(struct quadQueryReturn *thisQuery)
{
  free(thisQuery->boids);
  free(thisQuery);
}

