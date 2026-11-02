#include "global.h"
#include "player.h"
#include "engine.h"
#include "ram.h"
#include "render.h"

// confidence=low: chooses the nearest visible list entry within a fixed squared-distance limit.
#define Player_FindNearestVisibleTarget sub_0814C248
struct TargetPosition {
    s16 x, y, z;
};
struct TargetBody {
    u8 unknown[12];
    struct TargetPosition pos;
};
struct MotionTarget {
    struct TargetBody *body;
    u32 flags, unknown8;
    struct MotionTarget *next;
};
struct MotionTargets {
    u8 unknown[0x18];
    struct MotionTarget *first;
};
static inline s32 ProjectScale(s32 value)
{
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}
static inline void ProjectPosition(struct TargetPosition *screen,
                                   const struct TargetPosition *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 sum, vertical, y, depth;
    s32 scaledSum;
    screen->x = ProjectScale(3 * (x - z));
    scaledSum = ProjectScale(3 * (x + z));
    sum = scaledSum;
    vertical = ProjectScale(3 * world->y);
    y = sum - vertical;
    depth = sum + vertical;
    screen->x = (u16)screen->x - gCameraOrigin[0] + 120;
    screen->y = y - gCameraOrigin[1] + 80;
    screen->z = depth - gCameraOrigin[2];
}

struct MotionTarget *Player_FindNearestVisibleTarget(struct Player *arg)
{
    struct Player *p = arg;
    s32 distance = 1960000;
    struct MotionTarget *best;
    struct MotionTarget *entry;
    if (!gSceneLinkList)
        return 0;
    entry = gSceneLinkList->first;
    best = 0;
    if (entry)
        do {
            u32 loadedFlags = entry->flags;
            u32 disabled = loadedFlags & 1;
            u32 flags = loadedFlags;
            if (!disabled) {
                s32 x;
                s32 z;
                s32 square;
                struct TargetBody *body;
                if (flags & 16) {
                    body = entry->body;
                    x = body->pos.x - p->position.x;
                    z = body->pos.z - p->position.z;
                } else {
                    body = entry->body;
                    x = body->pos.x - p->position.x;
                    z = body->pos.z - p->position.z;
                }
                square = (x * x) + (z * z);
                if (square < distance) {
                    if (gEngineFlags & 0x1000) {
                        distance = square;
                        best = entry;
                    } else {
                        struct TargetPosition screen;
                        const struct TargetPosition *world;
                        if (flags & 16)
                            world = &entry->body->pos;
                        else
                            world = &entry->body->pos;
                        ProjectPosition(&screen, world);
                        if ((((u16)(screen.x - 1)) <= 238) && (((u16)(screen.y - 1)) <= 158)) {
                            distance = square;
                            best = entry;
                        }
                    }
                }
            }
            entry = entry->next;
        } while (entry);
    return best;
}
