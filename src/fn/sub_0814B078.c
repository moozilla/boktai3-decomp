#include "global.h"
#include "player.h"

// confidence=low: chooses the closest visible list entry and derives its travel duration.
// MATCHING_CONCESSION: register-pinning
#define Player_SelectNearestMotionTarget sub_0814B078
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
extern struct MotionTargets *gUnk_02000598;
extern u16 gUnk_03005418[];
extern u32 gUnk_030053F4;
u16 Sqrt(u32);
s32 Div(s32, s32);
static inline s32 Project(s32 input)
{
    s32 value = input * 3;
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}
static inline void ProjectPosition(struct TargetPosition *src, struct TargetPosition *dst)
{
    s32 x;
    s32 z;
    s32 depth;
    s32 height;
    s32 y;
    s32 projectedDepth;
    s32 projectedZ;
    asm("" : "+r"(src));
    x = src->x;
    z = src->z;
    dst->x = Project(x - z);
    depth = Project(x + z);
    projectedDepth = depth;
    height = Project(src->y);
    y = projectedDepth - height;
    projectedZ = projectedDepth + height;
    dst->x = (dst->x - gUnk_03005418[0]) + 120;
    dst->y = (y - gUnk_03005418[1]) + 80;
    dst->z = projectedZ - gUnk_03005418[2];
}

struct MotionTarget *Player_SelectNearestMotionTarget(struct Player *arg, u32 limit)
{
    struct Player *p = arg;
    u32 distance = limit;
    struct MotionTarget *best;
    struct MotionTarget *entry;
    if (!gUnk_02000598)
        return 0;
    entry = gUnk_02000598->first;
    best = 0;
    if (entry)
        do {
            register u32 rawFlags asm("r1") = entry->flags;
            u32 inactive = rawFlags & 1;
            u32 flags = rawFlags;
            if (!inactive) {
                s32 x;
                s32 z;
                u32 square;
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
                asm("" : "+r"(square));
                if (square < distance) {
                    if (!(gUnk_030053F4 & 0x1000)) {
                        struct TargetPosition screen;
                        {
                            u32 relative = flags & 16;
                            asm("" : : "r"(relative));
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
    *((u16 *)(((u8 *)p) + 0xA9C)) = Div(Sqrt(distance), *((u16 *)(((u8 *)p) + 0xA9E)));
    return best;
}
