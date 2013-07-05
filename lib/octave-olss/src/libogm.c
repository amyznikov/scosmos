/*
 * libogm.c
 *
 *  Created on: Mar 10, 2012
 *      Author: amyznikov
 */

#include "libogm.h"
#include "kahan.h"
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define DEBUG 0

struct ogmctx_t {
  double ll;    /*< The sum of RHS^2 of source equations, for least squares error estimation */
  double cll;   /*< The running compensation for lost low-order bits of ll */

  double * y;   /*< RHS vector of normal equation C*X=Y */
  double * cy;  /*< The running compensation for lost low-order bits of y[] */

  size_t n;     /*< Number of equations (individual measurements points) */
  size_t np;    /*< Number of system parameters to be found */
  double * c;   /* < matrix of normal equations */
  double * ci;  /*< inverse matrix of c */
};


#if DEBUG
# include <stdio.h>
static void pmatrix( size_t nr, size_t nc, const double m[] )
{
  size_t i, j;

  for ( i = 0; i < nr; ++i ) {
    for ( j = 0; j < nc; ++j ) {
      printf("%12g\t", m[i * nc + j]);
    }
    printf("\n");
  }
}
#endif


static inline double * alloc_vector( size_t size )
{
  return (double *) calloc(size, sizeof(double));
}

static double * alloc_matrix( size_t nrows, size_t ncols )
{
  return (double *) calloc(nrows * ncols, sizeof(double));
}

static inline void free_vector( double * v )
{
  if ( v ) {
    free(v);
  }
}

static void free_matrix( double * a )
{
  if ( a ) {
    free(a);
  }
}

/**
 * Multiply Matrix To Vector
 *  Y = M * X
 * @param nr  number of rows of matrix M, and size of vector Y
 * @param nc  number of columns of matrix M, and size of vector X
 * @param m   [input] matrix of size [nr x nc]
 * @param x   [input] vector of size nc
 * @param y   [output] vector of size nr
 */
static void mmv( size_t nr, size_t nc, const double m[/*nr*nc*/], const double x[/*nc*/], double y[/*nr*/] )
{
  size_t i, j;

  for ( i = 0; i < nr; ++i )
  {
    double s = 0, cs = 0;
    for ( j = 0; j < nc; ++j ) {
      /* s += m[i * nc + j] * x[j]; */
      safe_add(&s, &cs, m[i * nc + j] * x[j]);
    }
    y[i] = s;
  }
}

/**
 * Vector Scalar product
 */
static double mvv( size_t n, const double v1[/*n*/], const double v2[/*n*/] )
{
  double s = 0, cs = 0;
  while ( n-- ) {
    safe_add(&s, &cs, *v1++ * *v2++);
  }
  return s;
}


/**
 * Invert square matrix m[]
 * @param n     The size of matrix m[]
 * @param m     [Input] square matrix of size [n x n] to be inverted
 * @param mi    [Output] inverse matrix of size [n x n]
 * @return      0 on success, status code on error
 */
static int invert_matrix( ssize_t n, const double m[], double mi[] )
{
  const ssize_t n2 = 2 * n;
  ssize_t i, j, k;
  int status = OGM_SUCCESS;

  const ssize_t lda = n2;
  const ssize_t ldm = n;

  /* Allocate temporary work matrix of size [n x 2n] */
  double * a = 0;
  if ( !( a = alloc_matrix(n, n2) ) ) {
#if DEBUG
    fprintf(stderr,"alloc_matrix(%zd x %zd) fails: %s\n",n, n2, strerror(errno));
#endif
    return (status = OGM_MALLOC);
  }

  /* Copy source matrix into left half of work matrix */
  for ( i = 0; i < n; ++i ) {
    memcpy(a + i * lda, m + i * ldm, n * sizeof(double));
  }

  /* Setup diagonal elements of right half of work matrix */
  for ( i = 0; i < n; ++i ) {
    a[i * lda + n + i] = 1;
  }

  /* Invert matrix */
  for ( i = 0; i < n; ++i )
  {
    k = i;

    while ( k < n && a[k * lda + i] == 0 ) {
      ++k;
    }

    if ( k == n ) {
      break;
    }

    if ( k != i )
    {
      for ( j = 0; j < n2; ++j ) {
        double temp = a[k * lda + j];
        a[k * lda + j] = a[i * lda + j];
        a[i * lda + j] = temp;
      }
    }

    for ( j = n2 - 1; j >= i; --j ) {
      a[i * lda + j] /= a[i * lda + i];
    }

    for ( k = 0; k < n; ++k ) {
      if ( k != i ) {
        for ( j = n2 - 1; j >= i; --j ) {
          a[k * lda + j] = a[k * lda + j] - a[i * lda + j] * a[k * lda + i];
        }
      }
    }

  }

  if ( i != n )
  {
#if DEBUG
    fprintf(stderr, "Singular matrix ?\n i=%zd n=%zd\n", i, n);
    pmatrix(n, n2, a);
#endif
    status = OGM_SIGNULAR_MATRIX;
  }
  else
  {
    /* Ok, copy and return */
    for ( i = 0; i < n; ++i ) {
      memcpy(mi + i * ldm, a + i * lda + n, n * sizeof(double));
    }
  }

  free_matrix(a);
  return status;
}

/*
 * Solves a general system of linear equations C * X = Y.
 *
 * @param n The size of square matrix C and vectors X and Y
 * @param c Input square matrix of size [n x n]
 * @param y RHS vector of size [n]
 * @param x Output vector of size [n]
 * @param ci Output inverse matrix of c
 * @return 0 on success, integer error code on error
 *
 * @note Consider BLAS level3 to solve the system and get the inverse matrix
 */
static int lsolve( size_t n, const double * c, const double y[], double x[], double ci[] )
{
  int status;

  /* CI = invert(C) */

#if DEBUG
  fprintf(stderr, "-------------------------------------\n");
  fprintf(stderr, "source matrix: %zu x %zu\n", n, n);
  pmatrix(n, n, c);
  fprintf(stderr, "-------------------------------------\n");
#endif

  if ( ( status = invert_matrix(n, c, ci) ) != 0 ) {
#if DEBUG
    fprintf(stderr,"invert_matrix() fails: %d. Matrix size: %zu x %zu\n", status, n, n);
#endif
  }
  else {
    /* X = CI * Y */
    mmv(n, n, ci, y, x);
  }

  return status;
}

ogmctx_t * ogm_solver_create( size_t np )
{
  ogmctx_t * ctx = 0;

  if ( np < 1 ) {
    errno = EINVAL;
    return NULL;
  }

  if ( ( ctx = (ogmctx_t *) calloc(1, sizeof(ogmctx_t)) ) )
  {
    ctx->np = np;

    if ( !( ctx->c = alloc_matrix(np, np) ) ) {
      ogm_solver_destroy(ctx), ctx = 0;
    }
    else if ( !( ctx->ci = alloc_matrix(np, np) ) ) {
      ogm_solver_destroy(ctx), ctx = 0;
    }
    else if ( !( ctx->y = alloc_vector(np) ) ) {
      ogm_solver_destroy(ctx), ctx = 0;
    }
    else if ( !( ctx->cy = alloc_vector(np) ) ) {
      ogm_solver_destroy(ctx), ctx = 0;
    }
  }

  return ctx;
}

void ogm_solver_destroy( ogmctx_t * ctx )
{
  if ( ctx ) {
    free_vector(ctx->y);
    free_vector(ctx->cy);
    free_matrix(ctx->ci);
    free_matrix(ctx->c);
    free(ctx);
  }
}

size_t ogm_solver_get_np(ogmctx_t * ctx )
{
  return ctx->np;
}

size_t ogm_solver_get_n(ogmctx_t * ctx )
{
  return ctx->n;
}

int ogm_solver_append_tuple( ogmctx_t * ctx, const double a[/*np*/], double rhs )
{
  size_t i, j;
  const size_t ldc = ctx->np;

  safe_add(&ctx->ll, &ctx->cll, rhs * rhs); /* ctx->ll += rhs * rhs */
  ++ctx->n;

  for ( i = 0; i < ctx->np; ++i )
  {
    safe_add(&ctx->y[i], &ctx->cy[i], a[i] * rhs); /* ctx->y[i] += a[i] * rhs; */

    for ( j = 0; j < ctx->np; ++j ) {
      ctx->c[i * ldc + j] += a[i] * a[j];
    }
  }

  return 0;
}

int ogm_solve(ogmctx_t * ctx, double x[/*np*/], double e[/*np*/], double c[/*np * np */], double ci[/*np * np */],
    double * sigma)
{
  int status;

  const size_t np = ctx->np;

  if ( c != NULL ) {
    memcpy(c, ctx->c, np * np * sizeof( *c ));
  }

  /* Solve the C * X = Y */
  if ( ( status = lsolve(np, ctx->c, ctx->y, x, ctx->ci) ) == OGM_SUCCESS )
  {
    if ( ci != NULL ) {
      memcpy(ci, ctx->ci, np * np * sizeof( *ci ));
    }

    if ( e != NULL || sigma != NULL )
    {
      double ss, vv;
      size_t i;

      const size_t n = ctx->n;
      const size_t ldc = np;

      mmv(np, np, ctx->c, x, e);

      vv = mvv(np, x, e);

#if DEBUG
      if (ctx->ll < vv ) {
        fprintf(stderr,"warning: ll < vv; ll:%.16f vv:%.16f\n",ctx->ll, vv);
      }
      else if (ctx->ll == vv ) {
        fprintf(stderr,"warning: ll == vv; ll:%.16f vv:%.16f\n",ctx->ll, vv);
      }
#endif

      ss = ctx->ll > vv ? ( ctx->ll - vv ) / ( n - np ) : 0;

      if ( sigma != NULL ) {
        *sigma = ss;
      }

      if ( e != NULL ) {
        for ( i = 0; i < np; ++i) {
          e[i] = sqrt(ctx->ci[i * ldc + i] * ss);
        }
      }
    }
  }

  return status;
}
