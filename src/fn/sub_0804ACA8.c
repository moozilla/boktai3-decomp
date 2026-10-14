#include "tether_actor.h"
void TetherActor_AccumulateCharge(struct TetherActor *, u32);
struct TetherChargeContactView {
    u8 unknown0[6];
    u16 flags;
    u8 unknown8[2];
    u16 typeFlags;
    u8 unknownC[0x28];
    u32 responseFlags;
    u8 unknown38[4];
    u16 power;
};
void TetherActor_OnChargeContact(struct TetherChargeContactView *collider, void *other,
                                 struct TetherActor *inputActor)
{
    struct TetherActor *owner = inputActor;
    struct TetherActor *actor;
    u32 allowed;
    u32 responseMask;
    u32 flags;
    u32 mask = 0x800;
    u32 amount, cooldown;
    if (collider->flags & mask)
        goto done;
    actor = owner;
    if (actor->unknown1B == 1)
        goto done;
    allowed = 0;
    if (!actor->kindValue) {
        if (collider->typeFlags & 1)
            goto charge;
        if (!(collider->responseFlags & 512))
            goto allowGate;
        goto charge;
    } else
        allowed = 1;
allowGate:
    if (!allowed)
        goto done;
charge:
    if (actor->contactCooldown || !collider->power)
        goto done;
    responseMask = 1;
    flags = collider->responseFlags;
    responseMask &= flags;
    if (responseMask) {
        amount = 200;
        cooldown = 8;
    } else if (flags & 2) {
        amount = 300;
        cooldown = 12;
    } else if (flags & 4) {
        amount = 500;
        cooldown = 8;
    } else if ((flags &= 16) != 0) {
        amount = 500;
        cooldown = 8;
    } else {
        amount = 400;
        cooldown = 8;
    }
    TetherActor_AccumulateCharge(actor, amount);
    actor->paletteOverrideTimer = 4;
    actor->contactCooldown = cooldown;
done:;
}
