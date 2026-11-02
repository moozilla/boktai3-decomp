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
// MATCHING_CONCESSION: register-pinning

struct MotionTarget *TargetRegistry_FindById(u32 id)
{
    register u32 requestedId asm("r3") = id;
    struct MotionTargets *registry = gSceneLinkList;
    if (registry) {
        struct MotionTarget *entry = registry->first;
        /* An ID mismatch repeats the same entry, as in the original routine. */
        if (entry) {
            do {
                u32 entryId;
                u32 flags = entry->flags;
                u32 sceneActor = flags & 0x10;
                u32 disabled = flags & 1;
                if (!sceneActor)
                    goto actorId;
                entryId = entry->target.collider->collisionId;
            compareId:
                if (entryId != requestedId)
                    continue;
                if (!disabled)
                    return entry;
                break;
            actorId:
                entryId = entry->target.actor->id;
                goto compareId;
            } while (entry);
        }
    }
    return NULL;
}
