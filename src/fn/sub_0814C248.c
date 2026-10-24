#include "global.h"
#include "player.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: projection temporaries, repeated flag test and register allocation.
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
static inline void ProjectPosition(struct TargetPosition *dst, struct TargetPosition *src)
{
    s32 x, z;
    s32 depth, height, y;
    asm("" : "+r"(src));
    x = src->x;
    z = src->z;
    dst->x = Project(x - z);
    depth = Project(x + z);
    height = Project(src->y);
    y = depth - height;
    z = depth + height;
    dst->x = dst->x - gUnk_03005418[0] + 120;
    dst->y = y - gUnk_03005418[1] + 80;
    dst->z = z - gUnk_03005418[2];
}
inline static void ProjectPosition_0814C248(struct TargetPosition *dst, struct TargetPosition *src)
{
    s32 x;
    s32 z;
    u8 depth;
    s32 height;
    s32 y;
    typeof(z) t1;
    asm("" : "+r"(src));
    x = src->x;
    z = src->z;
    dst->x = Project(x - z);
    depth = Project(x + z);
    height = Project(src->y);
    y = depth - height;
    z = (t1 = depth + height);
    dst->x = (dst->x - gUnk_03005418[0]) + 120;
    dst->y = (y - gUnk_03005418[1]) + 80;
    dst->z = t1 - gUnk_03005418[2];
}

struct MotionTarget *Player_FindNearestVisibleTarget(struct Player *arg)
{
    register struct Player *p asm("ip") = arg;
    register s32 distance asm("sb") = 1960000;
    register struct MotionTarget *best asm("sl");
    register struct MotionTarget *entry asm("r6");
    if (!gUnk_02000598)
        return 0;
    entry = gUnk_02000598->first;
    best = 0;
    if (entry)
        do {
            register u32 flags asm("r4") = entry->flags;
            if (!(flags & 1)) {
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
                    if (!(gUnk_030053F4 & 0x1000)) {
                        struct TargetPosition screen;
                        {
                            u32 relative = flags & 16;
                            asm("" : : "r"(relative));
                        }
                        ProjectPosition_0814C248(&screen, &body->pos);
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
    asm("" : "+r"(best), "+r"(p), "+r"(distance));
    return best;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0814C248);
#endif
