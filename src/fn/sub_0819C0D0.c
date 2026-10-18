#include "global.h"
#include "contact_actor.h"
struct Hresvelgr {
    u8 unknown0[0xBC];
    u32 contactMask, contactFlags;
    u8 unknownC4[0x1E];
    u8 contactCue;
    u8 unknownE3[0x231];
    u16 responseValue;
    u8 unknown316[2];
    u16 contactBlocked;
    u8 unknown31A[2];
    u16 contactMagnitude;
    u8 unknown31E[2];
    u16 responseFlags;
    u8 unknown322[4];
    u16 contactPending;
    u8 contactSpecial, unknown329, contactDirection;
    u8 unknown32B[0x4D];
    s16 contactShift, contactParameter;
};
void sub_0821FF8C(void *, void *);
void PlayerStats_AddContactHundredths(void *, void *, u32);
s32 Math_Atan2(s32, s32);
static inline u32 testFlags(u16 *flags, u32 mask)
{
    return *flags & mask;
}
static inline u32 testReaction(u32 *flags, u32 mask)
{
    return *flags & mask;
}
void Hresvelgr_OnBodyContact(struct ContactCollider *a, struct ContactCollider *b,
                             struct Hresvelgr *actor)
{
    struct Hresvelgr *saved = actor;
    if (actor->contactBlocked == 0) {
        s32 magnitude, shift;
        sub_0821FF8C(a, b);
        magnitude = b->cost;
        shift = actor->contactShift;
        if (magnitude >= 0)
            magnitude >>= shift;
        else
            magnitude = -(-magnitude >> shift);
        actor->contactMagnitude = magnitude;
        actor->responseFlags = a->reactionFlags;
        if (testFlags(&a->flags, 0x100))
            actor->contactDirection = a->direction;
        else
            actor->contactDirection = Math_Atan2(b->x - a->x, b->z - a->z);
        {
            u32 mask = a->statRewardFlags;
            saved->contactMask = mask;
        }
        {
            u32 flags = a->reactionFlags;
            saved->contactFlags = flags;
        }
        PlayerStats_AddContactHundredths(a, b, saved->contactCue);
        if (testReaction(&a->reactionFlags, 0x20))
            actor->contactSpecial = 1;
        actor->contactPending = 1;
        actor->responseValue = a->responseValue;
        PlayerStats_AddContactHundredths(a, b, actor->contactParameter);
    }
}
