/*
 * libogm.h
 *
 *  Created on: Mar 10, 2012
 *      Author: amyznikov
 *
 *  Streamed linear least-squares solver
 */

#ifndef __libogm_h__
#define __libogm_h__

#include <stddef.h>
#include <inttypes.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif


#define OGM_SUCCESS           0
#define OGM_MALLOC            1
#define OGM_SIGNULAR_MATRIX   2

/**
 * Opaque typedef for ogm solver context
 */
typedef struct ogmctx_t ogmctx_t;

ogmctx_t * ogm_solver_create(size_t np);
void ogm_solver_destroy(ogmctx_t * ctx);
int ogm_solver_append_tuple(ogmctx_t * ctx, const double a[/*np*/], double rhs);
int ogm_solve(ogmctx_t * ctx, double x[/*np*/], double e[/*np*/], double c[/*np * np */], double ci[/*np * np */],
    double * ss);
size_t ogm_solver_get_np(ogmctx_t * ctx );
size_t ogm_solver_get_n(ogmctx_t * ctx );

#ifdef __cplusplus
}
#endif

#endif /* __libogm_h__ */
