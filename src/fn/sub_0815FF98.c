#include "global.h"

struct P {
    u8 f00[0x4C4];
    u32 w;
};

void sub_0815FA78(struct P *);
void Callback_InvokeActor(struct P *, u32);

s32 PlayerBurst_Update(struct P *p)
{
    sub_0815FA78(p);
    Callback_InvokeActor(p, p->w);
    return 0;
}
