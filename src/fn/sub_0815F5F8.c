#include "global.h"

struct TargetPosition {
    s16 x, y, z;
};
struct TargetBody {
    u8 unk_00[0xC];
    struct TargetPosition position;
};
struct TargetEntry {
    struct TargetBody *body;
    u32 flags;
    u32 unk_08;
    struct TargetEntry *next;
};
struct TargetRegistry {
    u8 unk_00[0x18];
    struct TargetEntry *first;
};
extern struct TargetRegistry *gUnk_02000598;
s32 Math_Atan2(s32, s32);

static inline s32 AngleDifference(const s32 angle, const s32 direction)
{
    s32 base = direction - 0x100;
    return (angle - base) & 0xFF;
}

struct TargetEntry *TargetRegistry_FindNearestInCone(struct TargetPosition *position, s32 direction, s32 range,
                                 s32 halfAngle)
{
    struct TargetEntry *entry;
    struct TargetEntry *best;
    s32 distance;
    s32 otherAngle;
    if (!gUnk_02000598)
        return 0;
    entry = gUnk_02000598->first;
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
                /* Both registered body layouts place their coordinates at the same offsets. */
                if (flags & 0x10) {
                    x = entry->body->position.x - position->x;
                    z = entry->body->position.z - position->z;
                } else {
                    x = entry->body->position.x - position->x;
                    z = entry->body->position.z - position->z;
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
