// MATCHING_CONCESSION: register-pinning
#include "contact_actor.h"
#include "player.h"
struct ContactModeActor {
    u8 unknown0[0x358];
    u32 mode;
};
void sub_0821FF8C(void *, void *);
void sub_08159860(void *, void *, u32);
struct ContactModeActor *sub_0802140C(s32);
void DamageNumberDisplay_Show(void *, u32, s32, void *, const struct ContactPackedVector *, u32);
void SceneActors_AddHealthCost(u32);
void sub_0806A160(void *, u32, const void *, u32, u32, u32, u32, u32);
extern const u8 gUnk_0824DE9C[];
void ContactActor_ApplyHealthContact(struct ContactCollider *sourceInput,
                                     struct ContactCollider *otherInput,
                                     struct ContactActor *ownerInput)
{
    register struct ContactCollider *source asm("r6") = sourceInput;
    register struct ContactCollider *other asm("r5") = otherInput;
    register struct ContactActor *owner asm("r4") = ownerInput;
    struct ContactPackedVector size;
    u32 directionMask;
    struct ContactPosition *position;
    u32 mode;
    u32 direction;
    struct ContactModeActor *actor;
    directionMask = 0x100;
    if (source->flags & directionMask)
        direction = source->direction;
    else
        direction = Math_Atan2(other->x - source->x, other->z - source->z);
    owner->direction = direction;
    mode = 0;
    if (owner->targetIndex >= 0) {
        owner->retargetTimer = 250;
        owner->targetIndex = mode;
    }
    sub_0821FF8C(source, other);
    sub_08159860(source, other, 2);
    actor = sub_0802140C(0);
    if (actor) {
        u32 playerMode = gUnk_02000580[0]->actionMode;
        if (actor->mode == 1 && playerMode != 2)
            other->cost = mode;
    }
    size.x = 16;
    size.y = 0;
    size.z = 16;
    {
        u32 cost = other->cost;
        position = &owner->position;
        DamageNumberDisplay_Show(other, cost, 0, position, &size, mode = 1);
    }
    SceneActors_AddHealthCost(other->cost);
    {
        u32 state = 5;
        u32 zero;
        zero = 0;
        owner->state = state;
        owner->enter = mode;
        owner->stateFrame = zero;
    }
    if (!other->cost) {
        Sound_PlaySongChecked(0x211);
    } else {
        Sound_PlaySong(0x108);
        if (source->reactionFlags & mode) {
            u32 index = (gUnk_03005308 + 1) & 1023;
            u32 angle;
            gUnk_03005308 = index;
            angle = ((const u8 *)RANDOM_TABLE_ADDR)[index * 2];
            sub_0806A160(position, 1, gUnk_0824DE9C, angle, 3, 8, 2, 2);
        }
    }
}
