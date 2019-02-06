#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "quadtree.h"
#include "utils.h"
void drawQuadtree(struct quadtree* thisQuad);
void drawRectange(struct vector2f position, float size, float width);
void drawBoid(struct boid *thisBoid);

#endif

