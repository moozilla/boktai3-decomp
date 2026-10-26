#include "contact_sequence_actor.h"
void ContactSequenceActor_Update(struct ContactSequenceOwner *, struct ContactSequenceActor *);
void Palette_InterpolateBanks(void *, void *, void *, u32, u32);
s32 ContactSequencePool_Update(struct ContactSequenceOwner *owner)
{
    struct ContactSequenceActor *actor;
    s32 remaining;
    owner->activeCount = 0;
    actor = owner->actors;
    remaining = 7;
    do {
        if (actor->active) {
            ContactSequenceActor_Update(owner, actor);
            if (actor->state == 2)
                ++owner->activeCount;
        }
        --remaining;
        ++actor;
    } while (remaining >= 0);
    if (owner->activeCount) {
        if (!owner->fadeReverse) {
            ++owner->fadeLevel;
            if (owner->fadeLevel > 31) {
                owner->fadeLevel = 32;
                owner->fadeReverse = 1;
            }
        } else {
            --owner->fadeLevel;
            if (owner->fadeLevel <= 0) {
                owner->fadeLevel = 0;
                owner->fadeReverse = 0;
            }
        }
        {
            u8 *bank = gPaletteBankData;
            u8 *first, *second;
            first = bank + 0x2C00;
            second = bank + 0x2CA0;
            Palette_InterpolateBanks(owner->palette, first, second, owner->fadeLevel, 5);
        }
    }
    return 0;
}
