#include <stdio.h>
#include <stdlib.h>

int main()
{
  int i;

  for (i=0; i<1000; ++i)
    printf("%d\n", random());

  return 0;
}
