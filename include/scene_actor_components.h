#ifndef GUARD_SCENE_ACTOR_COMPONENTS_H
#define GUARD_SCENE_ACTOR_COMPONENTS_H

#include "contact_position.h"

struct SceneActorChaseTarget {
    u8 unknown0[8];
    u8 playerIndex;
    u8 unknown9[3];
    struct ContactPosition position;
};

/* Body installed by SceneBody_Init at actor +0x50. */
struct SceneActorBody {
    u32 id;
    u32 flags;
    u8 mode, direction;
    u8 unknown5A[2];
    struct ContactPosition position;
    struct ContactPosition motion; /* +0x14: fourth halfword initializes to 0x10. */
    void *movementProbe;
    u16 probeExtentX, probeExtentZ;
    u32 field24;
    void *currentRoute;
    void *sprite;
    u8 unknown30[0xC];
    void *owner;
    u8 unknown40[8];
};

/* Script 'a' supplies four records in these parallel arrays. */
struct SceneActorAttackParameters {
    u16 first[4];
    u16 second[4];
    u16 parameter[4];
    u32 flags[4];
};

/* Shared targeting block at +0xDC. The saved heading at +0xE9 and the
 * script parameter at +0xEA are separate bytes. */
struct SceneActorChaseTracking {
    struct SceneActorChaseTarget *target;
    struct ContactPosition destination;
    u8 unknownC, heading, fieldEA, flags;
    u16 x, z;
    u32 distance, range, limit, zSquared;
};

/* Default control snapshot at actor +0x118. */
struct SceneActorDefaultControl {
    u32 behaviorFlags, statusFlags;
    u8 direction, state, phase, behavior;
    const void *control;
    u8 kind, unknown129[3];
    u16 heightOffset, motionFlags;
    u32 privateFlags;
};

typedef s32 (*SceneActorResultCallback)(void *actor);
typedef s32 (*SceneActorValueQuery)(void);
typedef void (*SceneActorAction)(void *actor);
typedef u32 (*SceneActorDefaultReaction)(void);
typedef void (*SceneActorCommandHandler)(void *actor, const u16 *command);
typedef u16 (*SceneActorSequenceQuery)(void *actor);
typedef u32 (*SceneActorAnimationQuery)(void *actor);
typedef u8 (*SceneActorContactPredicate)(void *actor);

/* Some engine callback slots carry more than one established return domain.
 * Opaque entries retain the older indirect-dispatch interface. */
union SceneActorAnimationQuerySlot {
    void *opaque;
    SceneActorAnimationQuery word;
    SceneActorSequenceQuery sequence;
    SceneActorValueQuery value;
};
union SceneActorContactQuerySlot {
    void *opaque;
    SceneActorContactPredicate predicate;
    SceneActorResultCallback result;
};
union SceneActorCommandTable {
    const void *opaque;
    SceneActorCommandHandler const *handlers;
};

#endif
