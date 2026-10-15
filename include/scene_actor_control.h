#ifndef GUARD_SCENE_ACTOR_CONTROL_H
#define GUARD_SCENE_ACTOR_CONTROL_H
#include "engine.h"
/* Tentative owner: an eight-slot scene controller that applies player/tether
 * parameters. These names describe observed consumers, not screen identity. */
struct SceneActorControlSlot {
    u8 unknown0[0x3E];
    s16 kind;
    u16 flags;
    s16 index, timer;
    u8 unknown46[2];
    s32 scene;
    u16 parameter;
    u8 unknown4E[0xAC - 0x4E];
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
