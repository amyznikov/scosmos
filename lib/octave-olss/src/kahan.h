/*
 * kahan.h
 *
 *  Created on: Sep 17, 2012
 *      Author: amyznikov
 *
 * @see http://en.wikipedia.org/wiki/Kahan_summation_algorithm
 *
 */

#ifndef __kahan_sum_h__
#define __kahan_sum_h__

#ifdef __cplusplus
extern "C" {
#endif


void safe_add( double * sum, double * c, double x );
void safe_addf( float * sum, float *c, float x );


#ifdef __cplusplus
}
#endif


#endif /* __kahan_sum_h__ */
