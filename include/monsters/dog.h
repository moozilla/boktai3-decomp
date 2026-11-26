#ifndef GUARD_DOG_H
#define GUARD_DOG_H

#include "monsters/chase_actor.h"

/* Dog's class fields past the chase actor, as far as its code reads them (provisional). */
struct Dog {
    struct ChaseActor chase;
    struct ContactCollider collider; /* the bite: Collider_SetAttackProperties arms it */
    u32 flags;
    /* Squared distances to the tracking target (tracking.distance) that pick the action. */
    u32 nearRangeSquared;    /* the near attack */
    u32 farRangeSquared;     /* the far attack (palette 1 and 3) */
    u32 leapRangeSquared;    /* keyword 'Y': the bite leaps within it */
    u32 retreatRangeSquared; /* within it the dog retreats */
    u32 chaseRangeSquared;   /* beyond it the dog chases */
    u16 actionTimer; /* counts down to the next queued action (flag 1) */
    u16 actionDelay, alternateActionDelay; /* keyword 'Y'; the alternate when motion flag 0x80 */
    u16 actionCooldown; /* Counts down beside the near and far attack timers. */
    u16 nearTimer;
    u16 farTimer;
    u16 nearDistance, farDistance;
    /* The item taken from the player's inventory on contact (InventorySlot_PickRandom);
     * slot -1 when none. */
    s16 stolenSlot, stolenKind, stolenParameter;
    u8 turnClockwise; /* the retreat turns +64 (else -64) per blocked try */
    u8 retreatCount;  /* retreats taken; past 3 the dog chases instead */
    u8 turnDelay;
    u8 itemDamagePercent; /* keyword 'd' (50): a healing item taken costs this share of health */
    u8 unknown6AA[2];
    struct ChaseRouteProbe routeProbe;
    struct ContactPosition destination;
    struct ContactPosition leapTarget; /* where the leap lands: the player when it starts */
};
typedef char DogSize[(sizeof(struct Dog) == 0x6E0) ? 1 : -1];

#endif // GUARD_DOG_H
