#include "global.h"
#include "scene_actor_chase.h"

u8 *Path_GetPointData(u8 *);
s32 Math_Atan2(s32, s32);

struct RouteCursor {
    u8 pad0[2];
    u8 index;
    u8 unk3;
    u8 *data;
    u16 *entry;
};

static inline u32 Route_Random(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)RANDOM_TABLE_ADDR);
}

static inline void Route_SelectRandom(u8 *cursor)
{
    struct RouteCursor *p = (struct RouteCursor *)cursor;
    struct RouteCursor *q = p;
    u8 n;
    n = Mod(Route_Random(), *p->data);
    if (n == p->index)
        n = Mod(n + 1, *p->data);
    q->index = n;
    q->unk3 = 0;
    q->entry = (u16 *)(Path_GetPointData(q->data) + q->index * 8);
}

static inline s32 Dot(struct ContactPosition *a, struct ContactPosition *b)
{
    return a->x * b->x + a->z * b->z;
}

/* Chase_CellAt, Chase_Height and Chase_SnapToRoute as this function spells them: the cell index is
   computed into x itself (x = Chase_CellIndex(x, z)), so x, the index and the result share one
   register (fix.py sp_result_into_arg). */
static inline s32 Chase_CellAt_(struct ContactPosition *position)
{
    s32 x = position->x >> 8;
    x = Chase_CellIndex(x, position->z >> 8);
    return x;
}

static inline u32 Chase_Height_(struct ContactPosition *position)
{
    return Chase_HeightAt(Chase_CellAt_(position), position);
}

static inline void Chase_SnapToRoute_(struct ContactPosition *p, u8 *route)
{
    p->x = (*(u16 **)(route + 8))[0];
    p->z = (*(u16 **)(route + 8))[1];
    p->y = Chase_Height_(p);
}

void sub_080B1FA8(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)(&actor->group);
    struct RouteCursor *route = (struct RouteCursor *)(((u8 *)actor) + 0x448);
    u8 selected = 0;
    struct ChaseTarget *t;
    struct ContactPosition dir;
    struct ContactPosition delta;
    u8 angle;
    u16 *entry;
    s32 i;
    u8 found;
    u8 *cursor;
    void *nav;
    t = *((struct ChaseTarget **)(((u8 *)actor) + 0xDC));
    angle = Math_Atan2(t->position.x - actor->body.position.x, t->position.z - actor->body.position.z);
    dir.x = (gUnk_086149C4[(angle + 64) & 255] * 10) >> 12;
    dir.z = (gUnk_086149C4[angle] * 10) >> 12;
    entry = (u16 *)Path_GetPointData(route->data);
    found = 0;
    for (i = 0; i < (*route->data); i++) {
        delta.x = entry[0] - actor->body.position.x;
        delta.z = entry[1] - actor->body.position.z;
        if (Dot(&dir, &delta) < 0) {
            u32 w = *((u32 *)entry);
            if (w == (*(*((u32 **)(group + 0x7C)))))
                continue;
            selected = i;
            found = 1;
            break;
        }
        entry += 4;
    }

    if (!found) {
        cursor = group + 0x74;
        Route_SelectRandom(cursor);
    } else {
        route->index = selected;
        route->unk3 = 0;
        route->entry = entry;
        cursor = group + 0x74;
    }
    Chase_SnapToRoute_(*((struct ContactPosition **)(group + 0x200)), cursor);
    nav = *((void **)(group + 0x1FC));
    if (!Chase_TryRoute(actor, nav, &actor->body.position,
                        *((struct ContactPosition **)(group + 0x200)))) {
        Chase_SetCtl(actor, SceneActorControl_OnIdle, 0);
        Chase_SetState(actor, 3, 0, 0);
    } else {
        (*((struct ContactPosition **)(group + 0x200)))->pad = 1;
    }
}
