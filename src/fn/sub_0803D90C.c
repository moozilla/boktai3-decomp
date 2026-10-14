// MATCHING_CONCESSION: register-pinning
#include "scene_pool.h"
#define ScenePool_ApplyMotionControls sub_0803D90C
void sub_0811EDE0(struct ScenePoolActor *);
static inline void ApplyMotionControls(struct ScenePoolEntry *entry, struct ScenePoolActor *actor)
{
    u32 controls;
    s32 blocked, vertical;
    {
        struct ScenePoolEntry *arg = entry;
        entry = arg;
    }
    controls = actor->controlFlags;
    vertical = actor->verticalSpeed;
    blocked = 1;
    if (vertical >= 0) {
        register s32 value asm("r1") = actor->restrictionMetric >> 12;
        blocked = (u32)(value | -value) >> 31;
    }
    if (!blocked) {
        if (controls & 0x200) {
            actor->timerB = 500;
            actor->timerA = 500;
            sub_0811EDE0(actor);
        }
        if (controls & 0x100)
            actor->forwardSpeed += entry->step;
        if (controls & 0x40)
            actor->lateralSpeed -= entry->turnStep;
        else if (controls & 0x80)
            actor->lateralSpeed += entry->turnStep;
        if (controls & 0x10)
            actor->forwardSpeed += entry->speed;
        else if (controls & 0x20)
            actor->forwardSpeed -= entry->speed;
    }
}

void ScenePool_ApplyMotionControls(struct ScenePoolActor *actor, struct ScenePoolEntry *entry)
{
    struct ScenePoolActor *owner = actor;
    struct ScenePoolActor **ownerRef = &owner;
    ApplyMotionControls(entry, *ownerRef);
}
