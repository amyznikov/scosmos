#include <stdio.h>

static void pdms(double deg)
{
  int sign = '+';
  int g, m;
  double s;

  if ( deg < 0 ) {
    sign = '-';
    deg = -deg;
  }

  g = (int)(deg);
  m = (int)((deg - g)*60);
  s = ((deg-g)*3600 - m * 60);

  printf("%c%.2d %.2d %6.3f", sign, g, m, s);
}

static void phms(double deg)
{
  double dh;
  int h, m;
  double s;

  dh = deg / 15;

  h = (int)(dh);
  m = (int)((dh - h)*60);
  s = ((dh-h)*3600 - m * 60);

  printf("%.2d %.2d %6.3f", h, m, s);
}

int main(int argc, char * argv[] )
{
  double deg;

  if ( argc < 2 ) {
    printf("Quickly convert RA/DEC from degrees to XX:MM:SS.SSS form \n");
    printf("  radec2xms [RA] [DEC]\n");
    return 1;
  }


  if ( argc > 1 )
  {
    if ( sscanf(argv[1],"%lf", &deg) != 1 ) {
      fprintf(stderr,"invalid arg %s\n", argv[1]);
      return 1;
    }
    phms(deg);
  }

  if ( argc > 2 )
  {
    if ( sscanf(argv[2],"%lf", &deg) != 1 ) {
      fprintf(stderr,"invalid arg %s\n", argv[2]);
      return 1;
    }

    printf("\t");
    pdms(deg);
  }

  printf("\n");
  return 0;
}
