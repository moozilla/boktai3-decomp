#include "global.h"
#include "ram.h"
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
    u8 unknown608[0x260];
    u8 scriptRequested;
    u8 unknown869[0x8B];
    u16 flags;
    u8 unknown8F6[0x16];
    u32 script;
};
extern u32 gEngineFlags;
void Hud_SetMode(s32);
void Script_Call(u32, void *);
static inline u32 MaskFlags(u8 *p, u32 mask)
{
    return mask & *p;
}
static inline void AddFlags(u32 mask, u16 *p)
{
    *p = mask | *p;
}
void MotionActor_RunPendingScript(struct MotionActor *actor)
{
    struct MotionActorState *state = &actor->state;
    if (actor->scriptRequested) {
        if (actor->script) {
            u32 mask = 0x800;
            if (!((gEngineFlags | gScenePauseFlags) & mask))
                Hud_SetMode(7);
            Script_Call(actor->script, 0);
        }
        actor->scriptRequested = 0;
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
    {
        u32 mask = 4;
        mask |= actor->flags;
        actor->flags = mask;
    }
}
