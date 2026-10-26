#include "global.h"
#include "render.h"

struct S30042A0 {
    u8 pad[0x12];
    u16 f12;
};
extern struct S30042A0 gUnk_030042A0;

void Video_SetRenderCallbacks(u16 a, u32 b, u32 c, u32 d)
{
    gUnk_030042A0.f12 = a;
    gProjectedSpriteCallback = b;
    gSpriteRenderCallback = c;
    gExtraRenderCallback = d;
}
