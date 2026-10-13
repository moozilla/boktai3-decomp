#include "global.h"

struct C {
    u8 f[8];
    s16 v;
};

void sub_0822AFE8(s32);
void Trigger_ConsumePending(void *, s32);

void sub_080636A4(void *a, void *b, struct C *c)
{
    sub_0822AFE8(c->v);
    Trigger_ConsumePending(b, 1);
}
