#include "global.h"
struct RisingActorState {
    u8 pad0[6];
    u16 flags;
};
struct RisingActor {
    u8 pad0[0x14];
    u8 controller[0x8C];
    struct RisingActorState state;
    u8 padA8[0x3AC - 0xA8];
};
struct RisingActorManager {
    u8 task[0x18];
    u32 activeSlots;
    u8 pad1C[0x1c];
    struct RisingActor actors[1];
};
void OrbitEffect_UpdateContraction(void);
void SunOrb_SetUpdateCallback(void *, void (*)(void));
static inline void RisingActorState_SetFlag4(struct RisingActorState *state)
{
    state->flags |= 4;
}
s32 sub_081FEC38(struct RisingActorManager *manager, s32 index)
{
    if (manager->activeSlots & (1 << index)) {
        RisingActorState_SetFlag4(&manager->actors[index].state);
        SunOrb_SetUpdateCallback(manager->actors[index].controller, OrbitEffect_UpdateContraction);
        return 0;
    }
    return -1;
}
