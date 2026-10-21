#define RisingActor_SetCallback SunOrb_SetUpdateCallback
#include "global.h"
struct RisingDrawable {
    u8 pad0[6];
    s16 flags;
};
struct RisingActor {
    s32 tick;
    u8 pad4[0x10];
    u8 controller[0x8c];
    struct RisingDrawable drawable;
    u8 padA8[0x4c];
    s16 fieldF4;
    u8 padF6[0x2ae];
    s32 height;
};
void OrbitEffect_FireVolley(void);
void SunOrb_SetUpdateCallback(void *, void (*)(void));
static inline void showDrawable(struct RisingDrawable *drawable)
{
    drawable->flags |= 4;
}
s32 Mod(s32, s32);
s32 RisingActor_LowerHeight(struct RisingActor *actor)
{
    if (Mod(actor->tick, 4) == 0)
        actor->height += -0x2000;
    if (actor->height <= 0x40000) {
        actor->fieldF4 = 1;
        showDrawable(&actor->drawable);
        SunOrb_SetUpdateCallback(actor->controller, OrbitEffect_FireVolley);
    }
    return 0;
}
