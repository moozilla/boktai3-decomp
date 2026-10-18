#include "global.h"

void sub_080FA754(void *);
void sub_080FAE50(void *);
void sub_080FB30C(void *);

s32 Axe_PrepareUpdate(void *p)
{
    sub_080FA754(p);
    sub_080FAE50(p);
    sub_080FB30C(p);
    return 1;
}
