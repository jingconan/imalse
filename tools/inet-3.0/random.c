/*                      Random Number Generator

Functions:
  myrandom() -- generate a uniformly distributed random var on [0, 1]
  random_uniform_real()
  random_uniform_int()
  random_poisson()
  random_exp_real()
  random_exp_int()
  random_erlang()
  random_normal
  random_reset()

Remain works
  When read Fishman book, should recheck all the algorithms.

*/
#ifndef lint
static char rcsid[] =
"@(#) $Id: random.c,v 1.1.1.1 2002/05/09 14:08:19 jwinick Exp $ (USC)";
#endif

#include        <stdio.h>
#include        <math.h>

#define MAIN 0
#define A       16807L          /* multiplier, 7**5 */
#define M       2147483647L     /* modulus, 2**31 - 1; both used in myrandom */

double rint(double);

static long seeds[64] = {     /* seeds for random streams */
               1973272912L,  747177549L,   20464843L,  640830765L,
               1098742207L,   78126602L,   84743774L,  831312807L,
                124667236L, 1172177002L, 1124933064L, 1223960546L,
               1878892440L, 1449793615L,  553303732L, 1348540613L,
                998064456L,  319340905L, 1858439546L, 2055126236L,
               1392773054L, 1234084134L,  247091381L,  903104502L,
                649101602L, 1075807989L,  712282831L,  485664729L,
                158570528L, 1291853259L,  563444767L,  745518912L,
               2018503049L, 1975266472L,  687514864L, 1418823682L,
                 75809031L,  279851527L,  178947498L, 1269904814L,
                 13647252L,  584002983L, 1925408247L, 1635412932L,
               1980852373L,   25918206L, 1375398212L,  241508321L,
               1469557700L, 1355003899L, 1957689937L,  850644262L,
               1638801628L, 1313470097L, 1997425128L, 1709501965L,
               1804802611L,  855036952L,  556754123L, 1597014452L,
                266719407L, 1415331372L, 1637091411L, 1504351011L};

/* make a copy of seeds for simulation reset or restart */
static long seeds_copy[64] = {
               1973272912L,  747177549L,   20464843L,  640830765L,
               1098742207L,   78126602L,   84743774L,  831312807L,
                124667236L, 1172177002L, 1124933064L, 1223960546L,
               1878892440L, 1449793615L,  553303732L, 1348540613L,
                998064456L,  319340905L, 1858439546L, 2055126236L,
               1392773054L, 1234084134L,  247091381L,  903104502L,
                649101602L, 1075807989L,  712282831L,  485664729L,
                158570528L, 1291853259L,  563444767L,  745518912L,
               2018503049L, 1975266472L,  687514864L, 1418823682L,
                 75809031L,  279851527L,  178947498L, 1269904814L,
                 13647252L,  584002983L, 1925408247L, 1635412932L,
               1980852373L,   25918206L, 1375398212L,  241508321L,
               1469557700L, 1355003899L, 1957689937L,  850644262L,
               1638801628L, 1313470097L, 1997425128L, 1709501965L,
               1804802611L,  855036952L,  556754123L, 1597014452L,
                266719407L, 1415331372L, 1637091411L, 1504351011L};

double
myrandom(int snum)
{
#ifdef __svr4__
#define libj_lrand _libj_lrand
#endif /*__svr4__*/
  extern long libj_lrand();
  long low;

  seeds[snum] = low = libj_lrand(seeds[snum]);
  return(low * 4.656612875e-10);
}


/* if sd > 0, set seed for stream n; return current seed.
 */
long
random_seed(long int sd, int n)
                                /* new seed */
{
  if (n < 0 || n > 63)
    printf("stream seed number error");
  if (sd) seeds[n] = sd;
  return(seeds[n]);
}

double
random_uniform_real(int sn, double a, double b)
                                /* stream number */
                                /* lower and upper bounds */
{
  if (a > b)
    printf("uniform-real argument error: a > b");
  return(a + (b - a) * myrandom(sn));
}


int
random_uniform_int(int sn, int a, int b)
                                /* lower and upper bounds, inclusively */
{
  double b2;

  if (a > b)
    printf("uniform-int argument error: a = %d > b = %d\n", a, b);
  b2 = (double)b - a + 1.0;
  b2 = (double) b2 * myrandom(sn) - 0.5;

  return(a + rint(b2));
}

long
random_poisson(int sn, int a)
                                /* average arrivals in unit time */
{
  long  n = -1;
  double e, p = 1.0;

  e = exp((double) -a);
  do {
    p *= myrandom(sn);
    n++;
    } while (p >= e);
  return(n);
}

double
random_exp_real(int sn, double m)
                                /* mean value */
{ 
  return(-m * log(myrandom(sn)));
}

int
random_exp_int(int sn, double m)
                                /* mean value */
{
  return((int) rint((double) - m * log(myrandom(sn))));
}

double
random_erlang(int sn, double m, double s)
                                /* mean and standard deviation */
{
  int   i, k;
  double z;

  if (s > m)
    printf("erlang argument error: s > m");

  z = m / s;
  k = (int) z * z;
  z = 1.0;
  for (i = 0; i < k; i++)
    z *= myrandom(sn);

  return(-(m/k) * log(z));
}

double
random_normal(int sn, double m, double s)
                        /* mean and standard deviation */
{
  double        v1, v2, w, z1;
  static double z2 = 0.0;

  if (z2 != 0.0) {
    z1 = z2;
    z2 = 0.0;
    }

  else {
    do {
      v1 = 2.0 * myrandom(sn) - 1.0;
      v2 = 2.0 * myrandom(sn) - 1.0;
      w = v1 * v1 + v2 * v2;
      } while (w >= 1.0);

    w = sqrt((-2.0 * log(w)) / w);
    z1 = v1 * w;
    z2 = v2 * w;
    }

  return(m + z1 * s);
}

/* restart the random stream
 */
void
random_reset(void)
{
  int   i;
  for (i = 0; i < 64; i++)
    seeds[i] = seeds_copy[i];
}

#if MAIN
main()
{
  int i;

  for (i = 0; i < 200000; i++) {
    /*myrandom(1);*/
    printf("%.3f\n", random_normal(1, 2.65, 6.55));
  }
}
#endif /*MAIN*/
