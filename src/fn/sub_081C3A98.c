#include "global.h"
#define SpriteController_Stop RegistryF3E4_SetHidden
struct SpriteController {
    u8 unknown0[0x3c];
    u8 scaleX, scaleY;
    u8 unknown3e[0xba];
    s32 timer;
};
void SpriteController_Stop(struct SpriteController *);
s32 __divsi3(s32, s32);
void SpriteController_AnimateShrinking(struct SpriteController *p)
{
    s32 scale;
    s32 duration = 8;
    s32 timer = p->timer;
    if (timer <= 0)
        scale = 0x40;
    else if (timer >= duration)
        scale = 0x26;
    else {
        s32 delta = -0x1a00;
        delta *= timer;
        delta = __divsi3(delta, duration);
        if (delta >= 0)
            delta >>= 8;
        else
            delta = -((-delta) >> 8);
        scale = delta + 0x40;
    }
    p->scaleX = scale;
    p->scaleY = scale;
    if (p->timer >= duration)
        SpriteController_Stop(p);
    else
        p->timer++;
}
