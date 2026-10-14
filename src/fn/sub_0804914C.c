#include "global.h"

void sub_08048F40(void *);
void sub_08049020(void *);
void TetherActor_UpdateSegments(void *);

void sub_0804914C(void *p)
{
    sub_08048F40(p);
    sub_08049020(p);
    TetherActor_UpdateSegments(p);
}
