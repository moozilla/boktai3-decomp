#include "global.h"
struct MotionActorState {
    u8 unknown0[0x68];
    u32 pending;
    u8 unknown6C[0x50E];
    u8 mode;
    u8 unknown57B[0x73];
    u8 flags;
};
struct MotionActor {
    u8 unknown0[0x18];
    struct MotionActorState state;
    u8 unknown608[0x25C];
    u32 scriptTimer;
    u8 scriptRequested;
    u8 unknown869[0x8B];
    u16 flags;
    u8 unknown8F6[0x16];
    u32 script;
};
extern u32 gUnk_030053F0, gEngineFlags;
void Hud_SetMode(s32);
void Script_Call(u32, void *);
void MotionActor_RunDelayedScript(struct MotionActor *actor)
{
    struct MotionActorState *state = &actor->state;
    if (actor->scriptRequested)
        actor->scriptRequested = 0;
    {
        u32 mask = 4;
        mask |= actor->flags;
        actor->flags = mask;
    }
    if (actor->scriptTimer > 60 && actor->script) {
        u32 mask = 0x800;
        if (!((gEngineFlags | gUnk_030053F0) & mask))
            Hud_SetMode(7);
        Script_Call(actor->script, 0);
        actor->script = 0;
    }
    {
        u8 *flags = &state->flags;
        s32 mask = ~1;
        u32 zero;
        mask &= *flags;
        zero = 0;
        *flags = mask;
        state->mode = zero;
        state->pending = zero;
    }
    actor->scriptTimer++;
}
