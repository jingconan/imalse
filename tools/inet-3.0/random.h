/* functions in random.c */
/*
 * @(#) $Id: random.h,v 1.1.1.1 2002/05/09 14:08:19 jwinick Exp $ (USC)
*/
#ifndef __random_h__
#define __random_h__

extern double myrandom(int snum);   /* generate uniformly distributed random var on [0, 1] */
extern long random_seed(long int sd, int n);          /* select a new seed for a stream */
extern double random_uniform_real(int sn, double a, double b);
extern int random_uniform_int(int sn, int a, int b);
extern long random_poisson(int sn, int a);
extern double random_exp_real(int sn, double m);
extern unsigned int random_exp_int(int sn, double m);
extern double random_erlang(int sn, double m, double s);
extern double random_normal(int sn, double m, double s);
extern void random_reset(void);

#define random_lognormal(sn, median, stdev) \
((median) * exp(((double) (stdev))*(random_normal((sn), 0.0, 1.0))))

#define random_pareto(sn, scale, shape) \
((double) (scale) * pow(myrandom(sn), -1.0/(double)(shape)))

#endif /*__random_h__*/
