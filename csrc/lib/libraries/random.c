#include "random.h"

static int do_rand(unsigned long *ctx)
{
#ifdef  USE_WEAK_SEEDING
  return ((*ctx = *ctx * 1103515245L + 12345L) % ((unsigned long)RAND_MAX + 1));
#else  
  long hi, lo, x;
  x = *ctx;
  if (x == 0)
    x = 123459876L;
  hi = x / 127773L;
  lo = x % 127773L;
  x = 16807L * lo - 2836L * hi;
  if (x < 0)
    x += 0x7fffffffL;
  return ((*ctx = x) % ((unsigned long)RAND_MAX + 1));
#endif  /* !USE_WEAK_SEEDING */
}

static unsigned long next = 1;

int rand(void)
{
  return do_rand(&next);
}
