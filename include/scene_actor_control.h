#ifndef GUARD_SCENE_ACTOR_CONTROL_H
#define GUARD_SCENE_ACTOR_CONTROL_H
#include "engine.h"
#include "contact_actor.h"
/* Tentative owner: an eight-slot scene controller that applies player/tether
 * parameters. These names describe observed consumers, not screen identity. */
struct SceneActorControlSprite {
    u32 flags;
    u8 active;
};
struct SceneActorControlSlot {
    u32 spriteFlags;
    u8 active, unknown5[2], priority;
    u8 unknown8[8];
    u16 tile;
    u8 unknown12[0xA];
    struct ContactPosition position;
    u8 unknown24[8];
    struct DisplaySequenceState sequence;
    s16 stateFrame, kind;
    u16 flags;
    s16 index, timer;
    u8 unknown46[2];
    s32 scene;
    u16 parameter, previousParameter;
    struct ContactCollider collider;
    struct ContactPosition savedPosition;
};
struct SceneActorControlManager {
    u8 unknown0[0x38];
    u32 used;
    struct SceneActorControlSlot slots[8];
};
static inline u8 SceneActorControl_IsUsed(struct SceneActorControlManager *p, u32 index)
{
    if (p->used & (1U << index))
        return 1;
    return 0;
}
#endif
