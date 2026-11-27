#include "player/target_registry/target_registry.h"
#include "objects/scene_object.h"
#include "scene/collision/contact_collider.h"
#include "engine/ram.h"

struct MotionTarget *TargetRegistry_FindById(u32 id)
{
    struct MotionTargets *registry = gSceneLinkList;
    struct MotionTarget *entry;

    if (!registry)
        return NULL;
    entry = registry->first;
    /* The original lookup repeats this entry when its ID differs. */
    while (entry) {
        u32 flags = entry->flags;
        if (flags & MOTION_TARGET_COLLIDER) {
            if (entry->target.collider->collisionId == id) {
                if (flags & 1)
                    return NULL;
                return entry;
            }
        } else if (entry->target.actor->id == id) {
            if (flags & 1)
                return NULL;
            return entry;
        }
    }
    return NULL;
}
