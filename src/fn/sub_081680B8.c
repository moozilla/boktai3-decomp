#include "global.h"
#include "engine.h"
struct S {
    u8 f[0x418];
    u8 v;
};
struct P {
    u8 f[0xA40];
    struct S *s;
};
s32 PauseMenu_IsEquipmentSelectionEnabled(struct P *p)
{
    if (gEngineFlags & 0x1000)
        goto zero;
    if ((u8)(p->s->v - 1) <= 1)
        goto one;
zero:
    return 0;
one:
    return 1;
}
