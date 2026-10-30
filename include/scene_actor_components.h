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
    u8 unknown54[5];
    u8 direction;
    u8 unknown5A[2];
    struct ContactPosition position;
    struct ContactPosition motion;
    u8 unknown6C[0xC];
    void *currentRoute;
    u8 unknown7C[0x1C];
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

typedef s32 (*SceneActorContextUpdate)(void *actor);
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
};
union SceneActorContactQuerySlot {
    void *opaque;
    SceneActorContactPredicate predicate;
};
union SceneActorCommandTable {
    const void *opaque;
    SceneActorCommandHandler const *handlers;
};

#endif
