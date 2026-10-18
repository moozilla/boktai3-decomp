#include "global.h"
struct S {
    u8 p[0x2c];
    u32 x;
};
extern u16 gUnk_030054BC;
void Callback_InvokeActor(struct S *, u32);
u32 Solar_UpdateTask(struct S *s)
{
    if (gUnk_030054BC == 0)
        Callback_InvokeActor(s, s->x);
    return 0;
}
