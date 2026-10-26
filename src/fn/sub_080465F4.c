#include "contact_sequence_actor.h"
#include "render.h"
void ContactSequenceActor_Initialize(struct ContactSequenceOwner *, struct ContactSequenceActor *);
s32 ContactSequencePool_Initialize(struct ContactSequenceOwner *owner, s32 initialHealth,
                                   u32 contactValue, s32 resourceValue, u32 script,
                                   u32 reactionValue)
{
    struct ContactSequenceActor *actor;
    s32 remaining;
    owner->initialHealth = initialHealth;
    owner->contactValue = contactValue;
    owner->resourceValue = resourceValue;
    owner->script = script;
    owner->reactionValue = reactionValue;
    owner->sequenceResource = ScriptProc_Start(0x922E, 0xC5E2);
    actor = owner->actors;
    remaining = 7;
    do {
        ContactSequenceActor_Initialize(owner, actor);
        --remaining;
        ++actor;
    } while (remaining >= 0);
    CpuSet(gPaletteBankData + 0x2C00, owner->palette, 0x04000008);
    return 0;
}
