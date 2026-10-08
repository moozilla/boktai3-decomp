#include "global.h"

struct C { u8 f[8]; s16 v; };

void sub_0822B1D8(s32);
void sub_08020D68(void *, s32);

void sub_08063730(void *a, void *b, struct C *c)
{
    sub_0822B1D8(c->v);
    sub_08020D68(b, 1);
}
