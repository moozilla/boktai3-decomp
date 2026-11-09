#include "global.h"
#include "scene_object.h"
#include "contact_collider.h"
#include "ram.h"
struct MotionTarget {
    union {
        struct SceneBody *actor;
        struct ContactCollider *collider;
    } target;
    u32 flags;
    void (*onTarget)(void);
    struct MotionTarget *next;
};
struct MotionTargets {
    u8 task[0x18];
    struct MotionTarget *first;
};

struct MotionTarget *TargetRegistry_FindById(u32 id)
{
    struct MotionTargets *registry = gSceneLinkList;
    if (registry) {
        struct MotionTarget *entry = registry->first;
        if (entry) {
            do {
                u32 flags = entry->flags;
                u32 sceneActor = flags & 0x10;
                u32 disabled = flags & 1;
                if (!sceneActor)
                    goto actorId;
                if (entry->target.collider->collisionId != id)
                    continue;
                if (!disabled)
                    return entry;
                break;
            actorId:
                if (entry->target.actor->id != id)
                    continue;
                if (!disabled)
                    return entry;
                break;
            } while (entry);
        }
    }
    return NULL;
}
