#include "global.h"

struct C {
    u8 f[8];
    s16 v;
};

void sub_0822AE7C(s32);
void Trigger_ConsumePending(void *, s32);

void sub_0806366C(void *a, void *b, struct C *c)
{
    sub_0822AE7C(c->v);
    Trigger_ConsumePending(b, 1);
}
