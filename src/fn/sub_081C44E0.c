#include "global.h"

struct S {
    u8 pad[0x1c];
    s16 v[3];
    u8 pad2[0x64 - 0x22];
    void (*f64)(void);
    s32 f68;
};
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void Registry6A37_UpdateFadeIn(void);

void Registry6A37_StartFadeIn(struct S *p)
{
    s16 *v = p->v;
    s16 a = 0x15e, b = 0xc4;
    s32 z = 0;
    p->v[0] = a;
    v[1] = b;
    v[2] = z;
    ScreenTransition_SendCommand(0, 6, 4, 4, 4, 0xFFFF, z);
    p->f64 = Registry6A37_UpdateFadeIn;
    p->f68 = z;
}
