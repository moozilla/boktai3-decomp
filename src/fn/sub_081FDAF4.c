#include "global.h"

// MATCHING_CONCESSION: register-pinning
struct RisingDrawable {
    u8 pad0[6];
    s16 flags;
};
struct RisingHeightState {
    u16 fieldF4;
    u8 padF6[0x2ae];
    s32 height;
};
struct RisingActor {
    u8 pad0[0x14];
    u8 controller[0x8c];
    struct RisingDrawable drawable;
    u8 padA8[0x4c];
    struct RisingHeightState state;
};
void OrbitEffect_FireVolley(void);
void SunOrb_SetUpdateCallback(void *, void (*)(void));
s32 RisingActor_AdvanceHeight(struct RisingActor *actor)
{
    register s32 zero asm("r4");
    register struct RisingHeightState *state asm("r2");
    actor->state.height += 0x2000;
    state = &actor->state;
    zero = 0;
    state->fieldF4 = zero;
    if (actor->state.height > 0x5ffff) {
        struct RisingDrawable *drawable = &actor->drawable;
        s32 mask = ~4;
        drawable->flags &= mask;
        SunOrb_SetUpdateCallback(actor->controller, OrbitEffect_FireVolley);
    }
    return 0;
}
