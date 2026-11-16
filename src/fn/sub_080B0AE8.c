#include "global.h"
#include "scene_actor_control_phase.h"

void Vorn_UpdateScriptedAnimation(struct ChaseActor *);
static inline u8 IsUnavailable(void *input)
{
    struct ChaseActor *p = input;
    if (p) {
        u32 mask = 0x40;
        if (p->base.motionFlags & mask)
            return 1;
        mask = 0x20000;
        if (p->base.statusFlags & mask)
            return 1;
        if (p->base.health <= 0)
            return 1;
    }
    return 0;
}
static inline u8 HasFlags(u16 *flags, u32 mask)
{
    if (*flags & mask)
        return 1;
    return 0;
}
static inline u32 MaskedFlags(struct ChaseActor *p, u32 mask)
{
    return p->base.motionFlags & mask;
}
static inline u8 TestContext(u8 *base, u32 mask, u16 **out)
{
    u16 *field = (u16 *)(base + 0x6D4);
    u32 value = *field & mask;
    *out = field;
    if (value)
        return 1;
    return 0;
}
void SceneActorPhase_TryBeginSpecialMotion(struct ChaseActor *p)
{
    u8 *base = (u8 *)p;

    if (!IsUnavailable(base) && *(s16 *)(base + 0x682) == 1 && p->base.controlKind != 0x29 &&
        !MaskedFlags(p, 0xE3BF)) {
        u16 *flags;
        if (!TestContext(base, 0xC, &flags) && HasFlags(flags, 2)) {
            u32 state = 0x18;
            u8 *field = &p->base.state2A8;
            u32 zero = 0;
            *field = zero;
            p->base.phase2A9 = zero;
            p->base.behavior2AA = state;
            p->base.counter2AC = zero;
            {
                u8 *ready = &p->base.field2B1;
                u32 one = 1;
                void *control;
                u32 *motionFlags;
                *ready = one;
                control = p->base.onStanding;
                p->base.field2B2 = one;
                p->base.stepDone = zero;
                p->base.controlKind = zero;
                p->base.target = control;
                p->base.field2C8 = zero;
                motionFlags = &p->base.pendingControlFlags;
                {
                    s32 mask = -2;
                    *motionFlags &= mask;
                }
                {
                    u32 duration;
                    u32 *slot = &p->base.pendingControl;
                    u32 callback = (u32)Vorn_UpdateScriptedAnimation;
                    duration = 0x29;
                    *slot = callback;
                    p->base.pendingControlData = zero;
                    p->base.pendingControlRelease = zero;
                    p->base.pendingControlKind = duration;
                    *motionFlags = one;
                }
            }
        }
    }
}
