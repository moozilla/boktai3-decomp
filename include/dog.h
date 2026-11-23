#ifndef GUARD_DOG_H
#define GUARD_DOG_H

#include "chase_actor.h"

/* Dog's class fields past the chase actor, as far as its code reads them (provisional). */
struct Dog {
    struct ChaseActor chase;
    struct ContactCollider collider; /* the bite: Collider_SetAttackProperties arms it */
    u32 flags;
    u32 speed;
    u32 alternateSpeed;
    u32 rangeSquared;
    u32 lastCell;
    u32 currentSpeed;
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

#endif // GUARD_DOG_H
