/*
 * kahan.c
 *
 *  Created on: Sep 17, 2012
 *      Author: amyznikov
 *
 * Kahan summation algorithm
 * @see http://en.wikipedia.org/wiki/Kahan_summation_algorithm
 *
 * Compile this module with optimization turned off !!!
 */

#include "kahan.h"

void safe_add( double * sum, double * c, double x )
{
  double y = x - *c;
  double t = *sum + y;
  *c = ( t - *sum ) - y;
  *sum = t;
}

void safe_addf( float * sum, float *c, float x )
{
  float y = x - *c;
  float t = *sum + y;
  *c = ( t - *sum ) - y;
  *sum = t;
}
