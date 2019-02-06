#ifndef QUADTREE_H
#define QUADTREE_H
#include "utils.h"
#include "boid.h"

#define QUADTREE_MAX_BOID_PER_CELL (8)

struct quadtree {
  // Pointers to this tree's subsections
  struct quadtree *topLeft;
  struct quadtree *topRight;
  struct quadtree *bottomLeft;
  struct quadtree *bottomRight;

  // Size and position
  struct vector2f position;
  float size;

  //Pointer array to hold the boids
  struct boid *boids[QUADTREE_MAX_BOID_PER_CELL];
  int boidCount;

  // To remember if this quad has split or not yet
  char hasSplit;
  
};

struct quadQueryReturn {
  struct boid **boids;
  int boidCount;
  int arrayCapacity;
};


struct quadtree* createQuadtree(struct vector2f position, 
    float size);

void quadtreeInsert(struct quadtree* thisQuad, 
    struct boid *thisBoid);

void quadtreeUpdate(struct quadtree * thisQuad);

int quadIntersects(struct quadtree *thisQuad,
    struct vector2f position, float size);

void quadQuery(struct quadQueryReturn *queryReturn, 
    struct quadtree *thisQuad, struct vector2f queryPosition, 
    float querySize);

struct quadQueryReturn *quadCreateReturnArray();

void quadQueryAddBoids(struct quadQueryReturn *thisReturn, 
    struct boid *boids[], struct vector2f queryPosition, 
    float querySize);

void quadtreeFree(struct quadtree *thisQuad);

void quadQueryFree(struct quadQueryReturn *thisQuery);

#endif
