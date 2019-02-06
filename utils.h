#ifndef UTILS_H
#define UTILS_H
struct vector2f {
  float x;
  float y;
};
float genRandom();
void seedRandom();
float lowPassfilter(float value, float cutoff);
#endif
