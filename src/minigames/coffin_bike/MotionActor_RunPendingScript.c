#include "global.h"
#include "engine/engine_pause.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "minigames/coffin_bike/coffin_bike.h"
void Hud_SetMode(s32);
void MotionActor_RunPendingScript(struct CoffinBike *actor)
{
    struct CoffinBikeMotion *state = &actor->motion;
    if (actor->active) {
        if (actor->eventScript) {
            if (!Engine_PauseBits(0x800))
                Hud_SetMode(7);
            Script_Call(actor->eventScript, 0);
        }
        actor->active = 0;
    }
    {
        u8 *flags = &state->damageFlags;
        s32 mask = ~1;
        u32 zero;
        mask &= *flags;
        zero = 0;
        *flags = mask;
        state->damageTimer = zero;
        state->palette.pendingDamage = zero;
    }
    {
        u32 mask = 4;
        mask |= actor->flags;
        actor->flags = mask;
    }
}
