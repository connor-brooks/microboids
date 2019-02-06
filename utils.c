#include "utils.h"
#include <time.h>
#include <stdlib.h>

void seedRandom(){
  srand(time(NULL)); // Move to utils, call at init
}

// Function to gen random num, move to utils.c
float genRandom(){
  int iRand = rand() % 100;
  float rand = (float) iRand / 100.0;
//  rand = (rand * 2.0) - 1.0;
  return rand;
}

float lowPassfilter(float value, float cutoff) {
  if(value <= 0) {
    if(value <= -cutoff) return -cutoff;
    else return value; 
  }

  else {
    if(value >= cutoff) return cutoff;
    else return value;

  }
}
