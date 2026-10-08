#include "global.h"

struct C { u8 f[8]; s16 v; };

void sub_0822B2F8(s32);
void sub_08020D68(void *, s32);

void sub_08063634(void *a, void *b, struct C *c)
{
    sub_0822B2F8(c->v);
    sub_08020D68(b, 1);
}
