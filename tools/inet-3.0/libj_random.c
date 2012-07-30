/*
 * Copyright (c) Xerox Corporation 1993. All rights reserved.
 *  
 * License is granted to copy, to use, and to make and to use derivative
 * works, provided that Xerox is acknowledged in all documentation
 * pertaining to any such copy or derivative work. The Xerox trade name
 * should not be used in any advertising without its written permission.
 *  
 * XEROX CORPORATION MAKES NO REPRESENTATIONS CONCERNING EITHER THE
 * MERCHANTABILITY OF THIS SOFTWARE OR THE SUITABILITY OF THIS SOFTWARE 
 * FOR ANY PARTICULAR PURPOSE.  The software is provided "as is" without
 * express or implied warranty of any kind.
 *  
 * These notices must be retained in any copies of any part of this
 * software.
 *
 * Author: Sugih Jamin (jamin@usc.edu)
*/
#ifndef lint
static char rcsid[]=
"@(#) $Id: libj_random.c,v 1.1.1.1 2002/05/09 14:08:20 jwinick Exp $ (jamin@usc.edu)";
#endif

#define __libj_rand_m__       0x7fffffff           /* 2^31 - 1 */
#define __libj_rand_p__       0x7ffffffe           /* period: 2^31 - 2 */
#define __libj_rand_n__       4.6566128752e-10     /* normalizer: 1/m */

/*
 * return a seed between 1 and the generator's period.
*/
long
libj_srand(seed)
     long seed;
{
  return (seed % __libj_rand_p__ + 1);
}

/*
 * Generate a periodic sequence of pseudo-random numbers with
 * a period of 2^31 - 2.  The generator is the "minimal standard"
 * multiplicative linear congruential generator of Park, S.K. and
 * Miller, K.W., "Random Number Generators: Good Ones are Hard to Find,"
 * CACM 31:10, Oct. 88, pp. 1192-1201.
 *
 * The algorithm implemented is:  Sn = (a*s) mod m.
 *   The modulus m can be approximately factored as: m = a*q + r,
 *   where q = m div a and r = m mod a.
 *
 * Then Sn = g(s) + m*d(s)
 *   where g(s) = a(s mod q) - r(s div q)
 *   and d(s) = (s div q) - ((a*s) div m)
 *
 * Observations:
 *   - d(s) is either 0 or 1.
 *   - both terms of g(s) are in 0, 1, 2, . . ., m - 1.
 *   - |g(s)| <= m - 1.
 *   - if g(s) > 0, d(s) = 0, else d(s) = 1.
 *   - s mod q = s - k*q, where k = s div q.
 *
 * Thus Sn = a(s - k*q) - r*k,
 *   if (Sn <= 0), then Sn += m.
 *
 * To test an implementation for A = 16807, M = 2^31-1, you should
 *   get the following sequences for the given starting seeds:
 *
 *   s0, s1,    s2,        s3,          . . . , s10000,     . . . , s551246 
 *    1, 16807, 282475249, 1622650073,  . . . , 1043618065, . . . , 1003 
 *    1973272912, 1207871363, 531082850, 967423018
 *
 * It is important to check for s10000 and s551246 with s0=1, to guard 
 * against overflow.
*/
#ifdef sparc
/*
 * The sparc assembly code is based on Carta, D.G., "Two Fast
 * Implementations of the 'Minimal Standard' Random Number
 * Generator," CACM 33:1, Jan. 90, pp. 87-88.
 *
 * ASSUME that "the product of two [signed 32-bit] integers (a, sn)
 *        will occupy two [32-bit] registers (p, q)."
 * Thus: a*s = (2^31)p + q
 *
 * From the observation that: x = y mod z is but
 *   x = z * the fraction part of (y/z),
 * Let: sn = m * Frac(as/m)
 *
 * For m = 2^31 - 1,
 *   sn = (2^31 - 1) * Frac[as/(2^31 -1)]
 *      = (2^31 - 1) * Frac[as(2^-31 + 2^-2(31) + 2^-3(31) + . . .)]
 *      = (2^31 - 1) * Frac{[(2^31)p + q] [2^-31 + 2^-2(31) + 2^-3(31) + . . .]}
 *      = (2^31 - 1) * Frac[p+(p+q)2^-31+(p+q)2^-2(31)+(p+q)3^(-31)+ . . .]
 *
 * if p+q < 2^31:
 *   sn = (2^31 - 1) * Frac[p + a fraction + a fraction + a fraction + . . .]
 *      = (2^31 - 1) * [(p+q)2^-31 + (p+q)2^-2(31) + (p+q)3^(-31) + . . .]
 *      = p + q
 *
 * otherwise:
 *   sn = (2^31 - 1) * Frac[p + 1.frac . . .]
 *      = (2^31 - 1) * (-1 + 1.frac . . .)
 *      = (2^31 - 1) * [-1 + (p+q)2^-31 + (p+q)2^-2(31) + (p+q)3^(-31) + . . .]
 *      = p + q - 2^31 + 1
*/
#ifdef __svr4__
#define libj_lrand _libj_lrand
#endif /*__svr4__*/
extern long libj_lrand();
asm("\
  .global _libj_lrand              ;\
_libj_lrand:                       ;\
  sethi   %hi(16807), %o1          /* load Y register with multiplier */;\
  wr      %o1, %lo(16807), %y      ;\
  andcc   %g0, 0, %o4              /* clear N, V, and partial result */;\
  sethi   %hi(0x7fffffff), %o3     /* load 2^31 - 1 to a register */;\
  or      %o3, %lo(0x7fffffff), %o3;\
  mulscc  %o4, %o0, %o4            /* do multiplication in 16 steps */;\
  mulscc  %o4, %o0, %o4            /* 16 because 16807 < 2^16 */;\
  mulscc  %o4, %o0, %o4            /* see App. E of the Sparc Arch. Man. */;\
  mulscc  %o4, %o0, %o4            ;\
  mulscc  %o4, %o0, %o4            ;\
  mulscc  %o4, %o0, %o4            ;\
  mulscc  %o4, %o0, %o4            ;\
  mulscc  %o4, %o0, %o4            ;\
  mulscc  %o4, %o0, %o4            ;\
  mulscc  %o4, %o0, %o4            ;\
  mulscc  %o4, %o0, %o4            ;\
  mulscc  %o4, %o0, %o4            ;\
  mulscc  %o4, %o0, %o4            ;\
  mulscc  %o4, %o0, %o4            ;\
  mulscc  %o4, %o0, %o4            /* step 15th */;\
  mulscc  %o4, %g0, %o4            /* last step just shift */;\
  rd      %y, %o5                  /* getting q . . . */;\
  sll     %o4, 16, %o0             ;\
  srl     %o0, 1, %o0              ;\
  srl     %o5, 17, %o5             ;\
  or      %o0, %o5, %o0            /* got q */;\
  srl     %o4, 16, %o1             /* get p */;\
  addcc   %o0, %o1, %o0            /* sn = p + q */;\
  bvs     1f                       /* if sn >= 2^31 */;\
  and     %o3, %o0, %o1            /* sn = p+q - 2^31 */;\
  jmpl    %o7+8, %g0               /* ret from leaf */;\
  nop                              ;\
1:                                 ;\
  jmpl    %o7+8, %g0               /* ret from leaf */;\
  add     %o1, 1, %o0              /* sn += 1 */;\
");

long
libj_fishman(sn)
     long sn;
{ 
  long L, H;
  long p, q;
  long P;
  long Hi, Lo;

  L = sn & 0xffff;
  H = sn >> 16;
  
  q = 0xff5 * L;
  p = 0xff5 * H + (q >> 16);
  q = ((p & 0x7fff) << 16) | (q & 0xffff);
  P = H + (L >> 16);
  L = ((P & 0x7fff) << 16) | (L & 0xffff);
  Lo = q + ((L & 0x7fff) << 16);
  Hi = (p >> 15) + (P >> 15) + ((L & 0x7fff8000) >> 15);

  sn = Lo - __libj_rand_m__;
  sn += Hi;

  if (sn <= 0) {
    sn += __libj_rand_m__;
  }
  return (sn);
}
#else /* sparc */
#ifdef __small_endian__
#else
/*
 * The following is the C implementation of
 *   Sn = p + q,            if p+q < 2^31
 *   Sn = p + q - 2^31 + 1, otherwise
 * The calculations of p and q use the simulated double precision
 * operation described in MacDougall, M.H., "Simulating Computer
 * Systems Techniques and Tools," The MIT Press, 1987, pp. 230-235.
 *
 * The simulated double precision operation is like doing
 * a long-hand decimal multiplication.
 * Think of the 32-bit Sn as represented by two 16-bit H:L.
 * Let q = a*L + (a*H & 0x7fff) * 2^16
 *     p = (a*H + a*L/2^16) >> 15;
*/
long
libj_lrand(sn)
  long sn;
{
  long L, H;

  L = 16807 * (sn & 0xffff);
  H = 16807 * (sn >> 16);

  sn = ((H & 0x7fff) << 16) + L;   /* sn = q */
  sn -= __libj_rand_m__;           /* sn -= m */
  sn += H >> 15;                   /* sn += p */

  if (sn <= 0) {
    sn += __libj_rand_m__;
  }
  return (sn);
}

long
libj_fishman(sn)
     long sn;
{ 
  long L, H;
  long p, q;
  long P;
  long Hi, Lo;

  L = sn & 0xffff;
  H = sn >> 16;
  
  q = 0xff5 * L;
  p = 0xff5 * H + (q >> 16);
  q = ((p & 0x7fff) << 16) | (q & 0xffff);
  P = H + (L >> 16);
  L = ((P & 0x7fff) << 16) | (L & 0xffff);
  Lo = q + ((L & 0x7fff) << 16);
  Hi = (p >> 15) + (P >> 15) + ((L & 0x7fff8000) >> 15);

  sn = Lo - __libj_rand_m__;
  sn += Hi;

  if (sn <= 0) {
    sn += __libj_rand_m__;
  }
  return (sn);
}
#endif /* __small_endian__*/
#endif /*sparc*/

/*
 * To get random numbers between (0,1) let
 *   Un = Sn / m
*/
double
libj_drand(sn)
     long sn;
{
  return((double) libj_lrand(sn)*__libj_rand_n__);
}

/*
 * Combining two Multiplicative Linear Congruential Generators.
 * From Pierre L'ecuyer, "Efficient and Portable Combined Random
 * Number Generators," CACM 31:6, June '88, pp. 742-750.
 *
 *   Zn = Sum((-1)^(j-1) * Sn,j) mod (M1 - 1), 1 <= j <= l
 *
 * is uniform between 0 and M1 - 1.  The period of Zn is:
 *
 *    P <= [(M1 - 1) * (M2 -1) * . . . * (Ml - 1)]/2^(l-1)
 *
 * Here I use l = 2, thus:
 *
 *   Zn = Sn1 - Sn2
 *     if (Zn < 1), Zn += M1 - 1
 *
 * I define a combined MLCG using A of 48271 and 69621, both
 * with M = 2^31 - 1.  So the period is <= (2^61 - 2^31 + 2^-1)
*/ 
static long s1 = 1973272912L;
static long s2 = 747177549L;

long
libj_crand(sn)
     long sn;
{
  s1 = libj_lrand(s1);
  s2 = libj_fishman(s2);

  sn = s1 - s2;
  if (sn < 1) {
    sn += __libj_rand_p__;
  }
  return (sn);
}

/* ------------------ The rest are just for testing ---------------------- */

#ifdef MAIN
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>

/*
 * Seeds for random streams, the first 16 were from
 * Fishman, G.S., "Principles of Discrete Event Simulation," Wiley 1978.
*/
static long libj_seeds[64] = {     
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
  266719407L, 1415331372L, 1637091411L, 1504351011L
};

long
libj_randf(sn)
     long sn;
{
  register k = sn/44488;
  
  sn = 48271 * (sn - k *  44488) - 3399 * k;
  if (sn <= 0) { 
    sn += __libj_rand_m__;
  }
  return (sn);
}

long
libj_lecuyer(sn)
     long sn;
{
  register k = sn/54542;
  
  sn = 39373 * (sn - k * 54542) - 1481 * k;
  if (sn <= 0) { 
    sn += __libj_rand_m__;
  }
  return (sn);
}

/*
 * Two MLCGs with m != 2^31 - 1 from L'Ecuyer's paper.
*/
long
libj_randl1(sn)
     long sn;
{
  register k = sn/52774;
  
  sn = 40692 * (sn - k * 52774) - 3791 * k;
  if (sn <= 0) { 
    sn += 0x7fffff07;
  }
  return (sn);
}

/* lots of duplicated points? */
long
libj_randl2(sn)
     long sn;
{
  register k = sn/53668;
  
  sn = 40014 * (sn - k * 55668) - 12211 * k;
  if (sn <= 0) { 
    sn += 0x7fffffab;
  }
  return (sn);
}

/*
 * This is a general combinged mlcg driver.
 * It allows user to pick any two generators.
*/
static long (*rand1)();
static long (*rand2)();

long
libj_grand(sn)
     long sn;
{
  s1 = (*rand1)(s1);
  s2 = (*rand2)(s2);
  
  sn = s1 - s2;
  if (sn < 1) { 
    sn += __libj_rand_p__;
  }
  return (sn);
}

struct randomizer {
  char *name;
  long (*rand)();
} randsw[] = {
  { "lewis", libj_lrand},
  { "fishman", libj_fishman },
  { "lecuyer", libj_lecuyer },
  { "randf", libj_randf },
  { "randl1", libj_randl1 },
  { "randl2", libj_randl2 },
  { "combined", libj_grand },
  0 ,
};

void *
bind_randomizer(name)
     char *name;
{
  struct randomizer *rp;
  
  for (rp = randsw; rp->name && strcmp(rp->name, name); rp++);
  if (!rp->name) {
    fprintf(stderr, "%s: unknown randomizer\n", name);
    exit(-1);
  }
  return ((void *) rp->rand);
};

static void
usage(prog)
     char *prog;
{
  printf("Usage: %s -s <seed> -i <iteration> -r <randomizer> \
-f <first randomizer> -n <second randomizer> -t <dimension> -b<x0> -e<x1>\n",
         prog);
  return;
}

main(argc, argv)
     int argc;
     char *argv[];
{
  int i, op;
  extern char *optarg;
  extern int optind, opterr;
  char *prog;
  int x, y, z;
  long s = 1;
  int iteration = 10000;
  int trace = 0;
  double u, begin, end;
  long (*randgen)();
  struct timeval ts1, ts2;
  unsigned long min, sec, usec;
  double normalizer;
  
  prog = argv[0];
  
  randgen = libj_lrand;
  rand1 = libj_lrand;
  rand2 = libj_fishman;
  normalizer = __libj_rand_n__;
  
  begin = -1.0;
  end = 0.001;
  
  while ((op = getopt(argc, argv, "s:i:r:f:n:t:b:e:")) != EOF) {
    switch (op) {
    case 's': s = atol(optarg); break;
    case 'i': iteration = atoi(optarg); break;
    case 't': trace = atoi(optarg); break;
    case 'b': begin = atol(optarg); break;
    case 'e': end = atol(optarg); break;
    case 'r': randgen = bind_randomizer(optarg); break;
    case 'f':
      rand1 = bind_randomizer(optarg);
      randgen = libj_grand;
      break;
    case 'n':
      rand2 = bind_randomizer(optarg);
      randgen = libj_grand;
      break;
    default:
      usage(prog);
      exit(-1);
    }
  }

  if (randgen == libj_randl1 || rand1 == libj_randl1) {
    normalizer = 4.6566134130e-10;
  }
  if (randgen == libj_randl2 || rand1 == libj_randl2) {
    normalizer = 4.6566130573e-10;
  }
  if (randgen == libj_crand || randgen == libj_grand) {
    s1 = s;
  }
  
  if (trace) {
    x = 1; y = 0; z = 0;
    for (i = 0; i < iteration; i++) {
      s = (*randgen)(s);

      u = (double) s * normalizer;
      if (u <= end && x && u >= begin) {
        x = 0; y = 1;
        printf("%.6f\t", u);
      } else if (y) {
        y = 0;
        if (trace == 3) {
          z = 1;
          printf("%.6f\t", u);
        } else {
          x = 1;
          printf("%.6f\n", u);
        }
      } else if (z) {
        z = 0; x = 1;
        printf("%.6f\n", u);
      }
    }

    return (0);
  }
    
  gettimeofday(&ts1, 0);
  for (i = 0; i < iteration; i++) {
    s = (*randgen)(s);
  }
  gettimeofday(&ts2, 0);

  printf("s%d: %ld", iteration, s);
  if (iteration == 10000 && randgen == libj_lrand) {
    printf(" ?= 1043618065");
  }
  
  sec = ts2.tv_sec - ts1.tv_sec;
  if (sec) {
    usec = 1000000 - ts1.tv_usec + ts2.tv_usec;
    sec += usec/1000000;
    usec %= 1000000;
    min = sec/60;
    printf("\nelapsed time: %d:%d:%d:%d:%d (%.0f us)\n",
           min/60, min%60, sec%60, usec/1000, usec%1000, 
           (float)sec * 1000000 + usec);
  } else {
    usec = ts2.tv_usec - ts1.tv_usec;
    printf("\nelapsed time: 0:0:0:%d:%d\n", usec/1000, usec%1000);
  }

  return (0);
}
#endif /*MAIN*/

/*
 * Another thing to look at is the Spectral Test of the random variates.
 *   "It tends to measure the statistical independence of adjacent
 *   n-tuples of numbers," n = 2, 3, 4, . . . .
 * Knuth's vol. 2, pp. 98-100 (2nd ed.)
*/
