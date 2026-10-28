#include "global.h"
#include "runtime.h"
struct S {
    u8 p[0x2c];
    u32 x;
};
void Callback_InvokeActor(struct S *, u32);
u32 Solar_UpdateTask(struct S *s)
{
    if (gSolarPaused == 0)
        Callback_InvokeActor(s, s->x);
    return 0;
}
