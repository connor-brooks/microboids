#ifndef BOID_H
#define BOID_H
#include "quadtree.h"

struct boid{
  float x;
  float y;
  float xVelocity;
  float yVelocity;
  float sightArea;
};

struct boid* createBoid(float x, float y);

void boidUpdate(struct boid* thisBoid, 
    struct quadtree *thisQuadtree);

struct quadQueryReturn *getLocalBoids(struct boid* thisBoid,
    struct quadtree *thisQuadtree);

float boidGetX(struct boid* thisBoid);

float boidGetY(struct boid* thisBoid);

void boidVelocity(struct boid*, float x, float y);

void boidMove(struct boid* thisBoid);


void boidMoveAway(struct boid* thisBoid,
    struct quadQueryReturn *localBoids);

void boidMoveCloser(struct boid* thisBoid,
    struct quadQueryReturn *localBoids);

void boidMoveTogether(struct boid* thisBoid,
    struct quadQueryReturn *localBoids);

void boidAvoid(struct boid* thisBoid,
    struct quadQueryReturn *query);


void boidAttract(struct boid* thisBoid,
    struct quadQueryReturn *query);


void boidMatch(struct boid* thisBoid,
    struct quadQueryReturn *query);

void boidLimitVelocity(struct boid* thisBoid);

#endif
