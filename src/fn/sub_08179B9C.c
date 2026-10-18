#include "global.h"
#include "contact_actor.h"
struct CollisionReactionActor {
    u8 unknown0[0x51];
    u8 referenceDirection;
    u8 unknown52[0x7A];
    u8 direction;
    u8 unknownCD[0x397];
    u32 rewardFlags;
    u8 unknown468[9];
    u8 impactDirection;
    u8 unknown472[4];
    u8 frontalImpact;
};
s32 Math_Atan2(s32, s32);
static inline u32 TestColliderFlags(struct ContactCollider *source, u32 mask)
{ return source->flags & mask; }
void CollisionReaction_RecordImpact(struct ContactCollider *source, struct ContactCollider *target,
                  struct CollisionReactionActor *actor)
{
    s32 delta = actor->direction - actor->referenceDirection;
    s32 distance;
    u32 reward;
    if (delta > 128) delta -= 256;
    else if (delta < -128) delta += 256;
    if (delta < 0) delta = -delta;
    distance = delta;
    if (TestColliderFlags(source, 0x100)) actor->impactDirection = source->direction;
    else actor->impactDirection = Math_Atan2(target->x - source->x, target->z - source->z);
    if (distance <= 32) actor->frontalImpact = 1;
    reward = source->statRewardFlags;
    actor->rewardFlags = reward;
}
