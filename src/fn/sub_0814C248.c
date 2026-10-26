#include "global.h"
#include "player.h"
#include "engine.h"
#include "ram.h"

// confidence=low: chooses the nearest visible list entry within a fixed squared-distance limit.
// MATCHING_CONCESSION: register-pinning
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
extern u16 gUnk_03005418[];
static inline s32 Project(s32 input)
{
    s32 value = input * 3;
    if (value >= 0)
        value >>= 5;
    else
        value = -((-value) >> 5);
    return value;
}
static inline void ProjectPosition(struct TargetPosition *src, struct TargetPosition *dst)
{
    s32 x;
    s32 z;
    s32 depth;
    s32 height;
    s32 y;
    s32 projectedZ;
    asm("" : "+r"(src));
    x = src->x;
    z = src->z;
    dst->x = Project(x - z);
    depth = Project(x + z);
    height = Project(src->y);
    y = depth - height;
    projectedZ = depth + height;
    dst->x = (dst->x - gUnk_03005418[0]) + 120;
    dst->y = (y - gUnk_03005418[1]) + 80;
    dst->z = projectedZ - gUnk_03005418[2];
}

struct MotionTarget *Player_FindNearestVisibleTarget(struct Player *arg)
{
    struct Player *p = arg;
    register s32 distance asm("sb") = 1960000;
    register struct MotionTarget *best asm("sl");
    struct MotionTarget *entry;
    if (!gSceneLinkList)
        return 0;
    entry = gSceneLinkList->first;
    best = 0;
    if (entry)
        do {
            register u32 loadedFlags asm("r1") = entry->flags;
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
                    asm("" : "+r"(body));
                    z = body->pos.z - p->position.z;
                } else {
                    body = entry->body;
                    x = body->pos.x - p->position.x;
                    z = body->pos.z - p->position.z;
                }
                square = (x * x) + (z * z);
                if (square < distance) {
                    if (!(gEngineFlags & 0x1000)) {
                        struct TargetPosition screen;
                        {
                            u32 relative = flags & 16;
                            asm("" : "+r"(square) : "r"(relative));
                        }
                        ProjectPosition(&body->pos, &screen);
                        if ((((u16)(screen.x - 1)) > 238) || (((u16)(screen.y - 1)) > 158))
                            goto next;
                    }
                    distance = square;
                    best = entry;
                }
            }
        next:
            entry = entry->next;

        } while (entry);
    return best;
}
