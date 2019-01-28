#ifndef _RANDOM_H_INCLUDED
#define _RANDOM_H_INCLUDED

#define RAND_MAX 2000UL

static int do_rand(unsigned long *ctx);
int rand(void);

#endif
