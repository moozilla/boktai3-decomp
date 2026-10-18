#define SlotActor_ResetOnFlags sub_081FC8BC
#include "global.h"
#include "engine.h"
struct SlotActorSignal {
    u8 pad0[0x34];
    u32 flags;
};
struct SlotActorState {
    u8 pad0[0xa];
    u16 state;
};
struct SlotActor {
    u32 flags;
    u8 pad4[0x28];
    u16 phase;
    s8 heading;
    u8 pad2F;
    u32 action;
    u8 pad34[0x9e];
    u16 enabled;
    u8 padD4[0xa4];
    u16 timer;
};
void sub_081FC8BC(struct SlotActorSignal *signal, struct SlotActorState *state,
                  struct SlotActor *actor)
{
    if (signal->flags & 7) {
        Sound_PlaySong(0x170);
        actor->timer = 0;
        actor->heading -= 0x80;
        actor->flags = 0;
        actor->phase = 0;
        actor->action = 0x24;
        state->state = 1;
        actor->enabled = 1;
    }
}
