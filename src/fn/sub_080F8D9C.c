#include "global.h"
#include "rotation_phase.h"
void Sound_PlaySong(u32);
s32 Mod(s32, s32);
void SceneActorControl_ResetRotation(void);
static inline u16 ClearRotationFlag(u16 *flags, u32 mask)
{
    return mask & *flags;
}
void SceneActorControl_FinishRotation(struct ChaseActor *actor, s32 elapsed)
{
    u8 *state = actor->base.context;
    struct RotatingDrawable *drawable = actor->base.drawable;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        drawable->flags |= 2;
        state[0xEB1] = 40;
        Sound_PlaySong(0x1E3);
        actor->base.flags2C0 = ClearRotationFlag(&actor->base.flags2C0, ~0x1000);
    }
    if (Mod(elapsed, 6) == 0) {
        u32 mask = 32;
        u16 *flags = &actor->base.flags2D6;
        mask |= *flags;
        *flags = mask;
    }
    if (elapsed > 80)
        RotationPhase_Change(actor, SceneActorControl_ResetRotation, 52);
}
