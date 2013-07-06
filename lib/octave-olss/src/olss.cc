/*
 * olss.cc
 *
 *  Created on: Jul 4, 2013
 *      Author: amyznikov
 */

#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>
#include <iomanip>
#include <oct.h> // octave/
#include <octave/version.h>
#include "libogm.h"

#define UNUSED(x)     ((void)(x))
#define CTX_MAX       128

static ogmctx_t * ctx[ CTX_MAX ];

static int olss_create(int np)
{
  int sid;

  for ( sid = 0; sid < CTX_MAX; ++sid  )
  {
    if ( ctx[sid] == NULL )
    {
      if ( !(ctx[sid] = ogm_solver_create(np)) ) {
        octave_stdout << "error: memory allocation fails in ogm_solver_create()\n";
        std::flush(octave_stdout);
        sid = -1;
      }
      break;
    }
  }

  if ( sid == CTX_MAX ) {
    octave_stdout << "error: too many olss solvers\n";
    std::flush(octave_stdout);
    sid = -1;
  }

  return sid;
}

static void olss_destroy(int sid)
{
  ogm_solver_destroy(ctx[sid]);
  ctx[sid] = NULL;
}

static int getsid( const octave_value & arg )
{
  int sid = -1;
  if ( arg.is_scalar_type() ) {
    double v = arg.double_value();
    if ( ((sid = D_NINT(v)) != v) || sid < 0 || sid >= CTX_MAX || ctx[sid] == NULL ) {
      sid = -1;
    }
  }
  return sid;
}

static int olss_tuple(ogmctx_t * ctx, const NDArray & m, const NDArray & rhs)
{
  const int np = ogm_solver_get_np(ctx);
  const int nr = m.dim1();
  const double * pm = m.data();
  const double * pr = rhs.data();

  double tup[ np ];

  for ( int i = 0; i < nr; ++i )
  {
    /* F2C */
    for ( int j = 0; j < np; ++j ) {
      tup[j] = pm[j * nr + i];
    }

    ogm_solver_append_tuple(ctx, tup, pr[i]);
  }

  return 0;
}

static int olss_ctuple(ogmctx_t * ctx, const NDArray & m, const NDArray & rhs)
{
  const int np = ogm_solver_get_np(ctx);
  const int nr = m.dim2();
  const double * pm = m.data();
  const double * pr = rhs.data();

  for ( int i = 0; i < nr; ++i, pm += np ) {
    ogm_solver_append_tuple(ctx, pm, pr[i]);
  }

  return 0;
}

static inline int olss_solve(ogmctx_t * ctx, double x[/*np*/], double e[/*np*/], double c[/*np * np */],
    double ci[/*np * np */], double * ss)
{
  return ogm_solve(ctx, x, e, c, ci, ss);
}





/***********************************************************************************************************************
 *
 * function sid = olss_create(N)
 *
 **********************************************************************************************************************/
DEFUN_DLD( olss_create, args, nargout,
"-*- texinfo -*-\n"
"@deftypefn {Function} {@var{sid}} = olss_create(@var{NP})\n"
"\n"
"Create stream-oriented linear least squares solver.\n\n"
"\n"
"Usage:\n"
"\n"
"NP = 2\n\n"
"sid = olss_create(NP);\n\n"
"% Add row-wise (fortran style) tuples: \n\n"
"M = [ 1 2; 1 3; 1 4; 1 5]\n\n"
"R = [ 2; 4; 6; 8; 10]\n\n"
"olss_tuple(sid,M,R); \n"
"\n"
"% Add column-wise (C style) tuples (may be faster for large data blocks loaded from files): \n\n"
"M = [ 1 1 1 1 1 ; 6 7 8 9 10] \n\n"
"R = [ 12 14 16 18 20 ]\n\n"
"olss_ctuple(sid,M,R);\n"
"\n"
"% Solve equations: \n\n"
"[X,EX,S,C,CI] = olss_solve(sid)\n\n"
"% Destroy solver: \n\n"
"olss_destroy(sid);\n\n"
""
"@seealso{ olss_destroy(), olss_solve(), olss_tuple(), olss_ctuple() }\n\n\
   Copyright (c) 2013, Andrey Myznikov <andrey.myznikov@@gmail.com>\n\
@end deftypefn\n"
)
{
  int NP, sid = -1;
  octave_value_list retlist;

  if ( args.length() != 1 ) {
    error("olss_create(NP): missing mandatory argument NP");
  }
  else if ( !args(0).is_scalar_type() ) {
    error("olss_create(NP): NP must be positive integer scalar");
  }
  else
  {
    const double v = args(0).double_value();
    if ( ((NP = D_NINT(v)) != v) || (NP <= 0) ) {
      error( "olss_create(): NP must be positive integer scalar" );
    }
    else {
      sid = olss_create(NP);
    }
  }

  retlist(0) = sid;

  UNUSED(nargout);
  return retlist;
}

/***********************************************************************************************************************
 *
 * function olss_destroy(N)
 *
 **********************************************************************************************************************/
DEFUN_DLD( olss_destroy, args, nargout,
"-*- texinfo -*-\n\
@deftypefn {Function} olss_destroy(@var{sid})\n\
\n\
Destroy OLSS solver.\n\
\n\
\n\
Example:\n\n\
  See help olss_create();\n\
\n\
@seealso{ olss_create(), olss_solve(), olss_tuple(), olss_ctuple() }\n\n\
   Copyright (c) 2013, Andrey Myznikov <andrey.myznikov@@gmail.com>\n\
@end deftypefn\n"
)
{
  int sid;

  if ( args.length() != 1 ) {
    error("olss_destroy(sid): missing sid argument");
  }
  else if ( (sid = getsid(args(0))) == -1 ) {
    error("olss_destroy(sid): invalid olss handle");
  }
  else {
    olss_destroy(sid);
  }

  UNUSED(nargout);
  return octave_value_list();
}

/***********************************************************************************************************************
 *
 * function olss_tuple(M, RHS)
 *
 **********************************************************************************************************************/
DEFUN_DLD( olss_tuple, args, nargout,
"-*- texinfo -*-\n\
@deftypefn {Function} olss_tuple(@var{sid}, @var{m}, @var{rhs})\n\
\n\
Append row-wise (fortran style) tuple to olss solver.\n\
\n\
\n\
Example:\n\n\
  See help olss_create();\n\
\n\
@seealso{ olss_create(), olss_destroy(), olss_solve(), olss_ctuple() }\n\n\
   Copyright (c) 2013, Andrey Myznikov <andrey.myznikov@@gmail.com>\n\
@end deftypefn\n"
)
{
  int status = -1, sid, np;
  ogmctx_t * ctx;

  if ( args.length() != 3 ) {
    error("olss_tuple(sid,m,rhs): expected 3 arguments");
  }
  else if ( (sid = getsid(args(0))) == -1 || !(ctx = ::ctx[sid]) ) {
    error("olss_tuple(): invalid olss handle");
  }
  else if ( !args(1).is_float_type() || args(1).rows() < 1 ) {
    error("olss_tuple(): m must be non-empty real matrix");
  }
  else if ( !args(2).is_float_type() || args(2).columns() != 1 ) {
    error("olss_tuple(): rhs must be real column-vector");
  }
  else if ( args(2).rows() != args(1).rows() ) {
    error("olss_tuple(): number of rows of matrix m and vector rhs must match");
  }
  else if ( args(1).columns() != (np = ogm_solver_get_np(ctx)) ) {
    error("olss_tuple(): number of columns of matrix m must match to NP (=%d)", np);
  }
  else if ( (status = olss_tuple(ctx, args(1).float_array_value(), args(2).float_array_value())) ) {
    error("olss_tuple() fails");
  }

  UNUSED(nargout);
  return octave_value_list(1, status);
}

/***********************************************************************************************************************
 *
 * function olss_ctuple(M, RHS)
 *
 **********************************************************************************************************************/
DEFUN_DLD( olss_ctuple, args, nargout,
"-*- texinfo -*-\n\
@deftypefn {Function} olss_ctuple(@var{sid}, @var{m}, @var{rhs})\n\
\n\
Append column-wise (C-style) tuple to olss solver.\n\
\n\
\n\
Example:\n\n\
  See help olss_create();\n\
\n\
@seealso{ olss_create(), olss_destroy(), olss_solve(), olss_tuple() }\n\n\
   Copyright (c) 2013, Andrey Myznikov <andrey.myznikov@@gmail.com>\n\
@end deftypefn\n"
)
{
  int status = -1, sid, np;
  ogmctx_t * ctx;

  if ( args.length() != 3 ) {
    error("olss_ctuple(sid,m,rhs): expected 3 arguments");
  }
  else if ( (sid = getsid(args(0))) == -1 || !(ctx = ::ctx[sid]) ) {
    error("olss_ctuple(): invalid olss handle");
  }
  else if ( !args(1).is_float_type() || args(1).columns() < 1 ) {
    error("olss_ctuple(): m must be non-empty real matrix");
  }
  else if ( !args(2).is_float_type() || args(2).rows() != 1 ) {
    error("olss_ctuple(): rhs must be real row-vector");
  }
  else if ( args(2).columns() != args(1).columns() ) {
    error("olss_ctuple(): number of columns of matrix m and vector rhs must match");
  }
  else if ( args(1).rows() != (np = ogm_solver_get_np(ctx)) ) {
    error("olss_ctuple(): number of rows of matrix m must match to NP (=%d)", np);
  }
  else if ( (status = olss_ctuple(ctx, args(1).float_array_value(), args(2).float_array_value())) ) {
    error("olss_ctuple() fails");
  }

  UNUSED(nargout);
  return octave_value_list(1, status);
}


/***********************************************************************************************************************
 *
 * function olss_solve(sid)
 *
 **********************************************************************************************************************/
DEFUN_DLD( olss_solve, args, nargout,
"-*- texinfo -*-\n\
@deftypefn {Function} {@var{X},@var{EX},@var{S},@var{C},@var{CI}} = olss_solve(@var{sid})\n\
\n\
Solve equation system [A] * X = RHS in least squares sense\n\
\n\
\n\
Example:\n\n\
  See help olss_create();\n\
\n\
@seealso{ olss_create(), olss_destroy(), olss_tuple(), olss_ctuple() }\n\n\
   Copyright (c) 2013, Andrey Myznikov <andrey.myznikov@@gmail.com>\n\
@end deftypefn\n"
)
{
  int status, sid, n, np;
  ogmctx_t * ctx;
  octave_value_list outargs;

  if ( args.length() != 1 ) {
    error("olss_solve(sid): expected 1 argument");
  }
  else if ( (sid = getsid(args(0))) == -1 || !(ctx = ::ctx[sid]) ) {
    error("olss_solve(): invalid olss handle");
  }
  else if ( (n = ogm_solver_get_n(ctx)) < (np = ogm_solver_get_np(ctx)) ) {
    error("olss_solve(): not enough tuples (%d). need at least NP=%d", n, np );
  }
  else
  {
    double x[np], * px = 0;
    double e[np], * pe = 0;
    double ss, * pss = 0;
    double c[np * np], * pc = 0;
    double ci[np * np], * pci = 0;

    if ( nargout > 0 ) {
      px = x;
    }
    if ( nargout > 1 ) {
      pe = e;
    }
    if ( nargout > 2 ) {
      pss = &ss;
    }
    if ( nargout > 3 ) {
      pc = c;
    }
    if ( nargout > 4 ) {
      pci = ci;
    }


    switch ( status = olss_solve(ctx, px, pe, pc, pci, pss) )
    {
    case OGM_SUCCESS:
      if ( nargout > 0 ) {
        Matrix v(np, 1);
        memcpy(v.fortran_vec(), x, sizeof(x));
        outargs(0) = v;
      }
      if ( nargout > 1 ) {
        Matrix v(np, 1);
        memcpy(v.fortran_vec(), e, sizeof(e));
        outargs(1) = v;
      }
      if ( nargout > 2 ) {
        outargs(2) = ss;
      }
      if ( nargout > 3 ) {
        Matrix v(np, np);
        memcpy(v.fortran_vec(), c, sizeof(c));
        v.transpose();
        outargs(3) = v;
      }
      if ( nargout > 4 ) {
        Matrix v(np, np);
        memcpy(v.fortran_vec(), ci, sizeof(ci));
        v.transpose();
        outargs(4) = v;
      }
      break;

    case OGM_MALLOC:
      error("olss_solve() fails: not enough memory");
      break;

    case OGM_SIGNULAR_MATRIX:
      error("olss_solve() fails: singular matrix");
      break;

    default:
      error("olss_solve() fails: unknown error");
      break;

    }
  }

  return outargs;
}

