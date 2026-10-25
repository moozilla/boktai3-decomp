#include "global.h"
#include "scene_actor_chase.h"

u8 *Path_GetPointData(u8 *);
s32 Math_Atan2(s32, s32);
void Spider_SelectRandomEntry(u8 *cursor);

struct RouteCursor {
    u8 pad0[2];
    u8 index;
    u8 unk3;
    u8 *data;
    u16 *entry;
};

static inline s32 Dot(struct ContactPosition *a, struct ContactPosition *b)
{
    return a->x * b->x + a->z * b->z;
}

#ifdef NONMATCHING
// fuzzy: 95.7%
// scope shape 6 (differences once low registers are set aside)
// confidence=low: aims a route cursor at the first waypoint that lies ahead of the target, else picks a random one
void sub_080A4798(struct SceneActorRegistryResetView *actor)
{
    struct RouteCursor *route = (struct RouteCursor *)(((u8 *)actor) + 0x448);
    u8 *group = (u8 *)(&actor->group);
    u8 selected = 0;
    struct ChaseTarget *t;
    struct ContactPosition dir;
    struct ContactPosition delta;
    u8 angle;
    u16 *entry;
    s32 i;
    u8 found;

    t = *((struct ChaseTarget **)(((u8 *)actor) + 0xDC));
    angle = Math_Atan2(t->position.x - actor->position.x, t->position.z - actor->position.z);
    dir.x = (gUnk_086149C4[(angle + 64) & 255] * 10) >> 12;
    dir.z = (gUnk_086149C4[angle] * 10) >> 12;
    entry = (u16 *)Path_GetPointData(route->data);
    found = 0;
    for (i = 0; i < (*route->data); i++) {
        delta.x = entry[0] - actor->position.x;
        delta.z = entry[1] - actor->position.z;
        if (Dot(&dir, &delta) < 0) {
            u32 w = *((u32 *)entry);
            if (w == **((u32 **)(group + 0x7C)))
                continue;
            selected = i;
            found = 1;
            break;
        }
        entry += 4;
    }

    if (!found) {
        Spider_SelectRandomEntry(group + 0x74);
    } else {
        route->index = selected;
        route->unk3 = 0;
        route->entry = entry;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080A4798);
#endif
