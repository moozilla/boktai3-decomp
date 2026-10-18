#include "global.h"
struct P {
    void (*fn)(void);
    u32 w4;
    u32 w[3];
    u8 b14;
    u8 b15;
    u8 pad[2];
    u8 obj[1];
};
s32 SunClock_GetPhase(void);
s32 TimePalette_GetSourceIndex(s32);
void Palette_CopyBank(void *, u32);
void TimePalette_UpdateTransition(void);
void TimePalette_InitTransition(struct P *o, u32 a, u32 b, u32 c)
{
    u32 z;
    s32 r;
    r = TimePalette_GetSourceIndex(SunClock_GetPhase());
    z = 0;
    o->b14 = r;
    o->b15 = r;
    o->w[0] = a;
    o->w[1] = b;
    o->w[2] = c;
    Palette_CopyBank(o->obj, o->w[r]);
    o->fn = TimePalette_UpdateTransition;
    o->w4 = z;
}
