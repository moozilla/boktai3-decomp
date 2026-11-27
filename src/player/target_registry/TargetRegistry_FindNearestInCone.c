#include "global.h"
#include "engine/ram.h"
#include "player/target_registry/target_registry.h"
#include "objects/scene_object.h"
#include "scene/collision/contact_collider.h"

s32 Math_Atan2(s32, s32);

static inline s32 AngleDifference(const s32 angle, const s32 direction)
{
    s32 base = direction - 0x100;
    return (angle - base) & 0xFF;
}

struct MotionTarget *TargetRegistry_FindNearestInCone(struct SceneVec *position, s32 direction,
                                                      s32 range, s32 halfAngle)
{
    struct MotionTarget *entry;
    struct MotionTarget *best;
    s32 distance;
    s32 otherAngle;
    if (!gSceneLinkList)
        return 0;
    entry = gSceneLinkList->first;
    best = 0;
    distance = range * range;
    if (entry) {
        otherAngle = 0x100 - halfAngle;
        do {
            u32 flags = entry->flags;
            if (flags & 1) {
                s32 x;
                s32 z;
                s32 angle;
                s32 square;
                if (flags & MOTION_TARGET_COLLIDER) {
                    x = entry->target.collider->position.x - position->x;
                    z = entry->target.collider->position.z - position->z;
                } else {
                    x = entry->target.actor->position.x - position->x;
                    z = entry->target.actor->position.z - position->z;
                }
                angle = AngleDifference(Math_Atan2(x, z), direction);
                if (angle <= halfAngle || angle >= otherAngle) {
                    square = x * x + z * z;
                    if (square < distance) {
                        distance = square;
                        best = entry;
                    }
                }
            }
            entry = entry->next;
        } while (entry);
    }
    return best;
}
