#include "global.h"
struct ActorRecovery {
    u8 unknown0[0x68];
    u32 pendingDamage;
    u8 unknown6C[0x12];
    u16 recoveryValue;
    u8 unknown80[0x299];
    u8 actionKind;
    u8 unknown31A[0xA];
    u16 unknown324, recoveryTimer, cooldown;
    u8 unknown32A[0x250];
    u8 damageTimer;
    u8 unknown57B[0x73];
    s8 damageFlags;
};
void PalFade_Start(struct ActorRecovery *, void *, u32, s32, s32, s32, s32, s32);
static inline void ClearDamageFlag(s8 *flags, s32 mask)
{
    *flags &= mask;
}
s32 ActorAction_TickRecovery(struct ActorRecovery *actor, void *context)
{
    if (actor->actionKind == 1 && actor->recoveryTimer) {
        actor->recoveryTimer--;
        ClearDamageFlag(&actor->damageFlags, ~1);
        actor->damageTimer = 0;
        actor->pendingDamage = 0;
        if (!actor->recoveryTimer)
            PalFade_Start(actor, context, actor->recoveryValue, 0, 0, 16, 1, 1);
    }
    if (actor->cooldown)
        actor->cooldown--;
}
