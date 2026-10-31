#ifndef GUARD_SCENE_STEERING_ACTOR_H
#define GUARD_SCENE_STEERING_ACTOR_H
#include "contact_actor.h"
/* Tentative eight-slot steering actor pool, global02000188. The prefix is a
 * sequence sprite, followed by a shadow and the actor's own contact collider. */
struct SceneSteeringActor {
    u32 flags;
    u8 active, unknown5[2], priority;
    s8 scaleX, scaleY;
    u8 unknownA[2];
    void *graphics;
    u16 frame;
    u8 unknown12[0xa];
    struct ContactPosition position;
    u8 unknown24[8];
    struct DisplaySequenceState sequence;
    u8 shadow[0x6c];
    struct ContactCollider collider;
    struct ContactPosition velocity;
    struct ContactPosition target;
    u8 initialDirection, direction, steeringTimer, turnStep;
    u16 speed, parameter;
    u8 unknown114, shadowStatus;
    s16 scaleTimer;
    const void *sequenceResource;
    s32 lifetime;
    u16 flags120;
    u8 state;
    s8 index;
    u16 stateFrame;
    u8 enter, steeringDelay;
};
struct SceneSteeringActorPool {
    u8 task[0x18];
    u8 graphics[0x1c];
    const void *sequenceResource;
    u32 used;
    struct SceneSteeringActor slots[8];
};
/* 0x02000188 */
extern struct SceneSteeringActorPool *gSceneSteeringActorPool;
#endif
