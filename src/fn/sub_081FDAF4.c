#include "global.h"

#ifdef NONMATCHING
// fuzzy: 83.3%
// fix.py best version (reprinted C)
// foundry draft (worker best 81.1%); what differs is mostly register allocation and instruction order
#define RisingActor_SetCallback SunOrb_SetUpdateCallback
struct RisingDrawable {
    u8 pad0[6];
    s16 flags;
};
struct RisingActor {
    u8 pad0[0x14];
    u8 controller[0x8c];
    struct RisingDrawable drawable;
    u8 padA8[0x4c];
    u16 fieldF4;
    u8 padF6[0x2ae];
    s32 height;
};
void OrbitEffect_FireVolley(void);
void SunOrb_SetUpdateCallback(void *, void (*)(void));
s32 RisingActor_AdvanceHeight(struct RisingActor *actor)
{
    u16 zero;
    actor->height += 0x2000;
    zero = 0;
    actor->fieldF4 = 0;
    if (actor->height > 0x5ffff) {
        struct RisingDrawable *drawable = &actor->drawable;
        s32 mask = ~4;
        drawable->flags &= mask;
        SunOrb_SetUpdateCallback(actor->controller, OrbitEffect_FireVolley);
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", RisingActor_AdvanceHeight);
#endif
