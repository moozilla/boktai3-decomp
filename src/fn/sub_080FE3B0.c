#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"

#ifdef NONMATCHING
// fuzzy: 95.2%
// fix.py best version (reprinted C)
// confidence=low: after the wait, selects a route point and starts the stored movement spline.
// Remaining differences: pointer register allocation (target/copy/zero use r7/r8/r5 instead of r7/r5/r8).

void Axe_SelectRandomRoutePoint(void *);
void Axe_SetMovementEndpoints(void *, void *);
void sub_080FAA5C(void *);
void CpuSet(const void *, void *, u32);

struct AxeVec {
    s16 x, y, z;
};
struct AxeRoutePoint {
    s16 x, z;
};
struct AxeGroup {
    u8 unknown0[0x44];
    u16 field44;
    u8 unknown46[0x36];
    struct AxeRoutePoint *point;
    u32 has;
};
struct AxeLink {
    u8 unknown0[8];
    struct AxeRoutePoint *point;
};
struct AxeContext {
    u8 unknown0[0xF14];
    u32 clearF14[2];
};

static inline u32 CellIndex(struct AxeVec *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u32 CellHeight(struct AxeVec *position, u32 cell)
{
    u32 shape, height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    shape = *data >> 4;
    height = (*data & 15) << 8;
    if (shape == 1)
        goto slopeZ;
    if (shape == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *((u8 *)position + 4);
    goto done;
slopeX:
    height -= *(u8 *)position;
done:
    return height;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior, u32 counter)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = counter;
    actor->field2B1 = 1;
}

void Axe_UpdateRouteDestinationPhase(struct SceneActorRegistryResetView *actor)
{
    struct AxeContext *context = actor->context;
    u32 zero;
    vu32 tmp;
    struct AxeVec targetStorage;
    struct AxeVec *target;
    struct AxeVec *copy;
    struct AxeRoutePoint *point;
    struct AxeLink *link;
    typeof(target->x) t1;
    typeof(copy->z) t2;
    if (actor->field2B1)
        actor->field2B1 = 0;
    if ((u8)Callback_InvokeActor(actor, actor->update270))
        return;
    if (((s32)actor->counter2AC) > 60) {
        Axe_SelectRandomRoutePoint(actor);
        target = &targetStorage;
        link = (struct AxeLink *)(((u8 *)actor) + 0x448);
        point = link->point;
        copy = target;
        target->x = point->x;
        target->z = point->z;
        target->y = CellHeight(target, CellIndex(target));
        zero = 0;
        Axe_SetMovementEndpoints(actor, copy);
        sub_080FAA5C(actor);
        SetState(actor, 0, 0, 1, zero);
        {
            struct AxeVec *position = (struct AxeVec *)(&actor->position);
            target->x -= position->x;
            copy->y -= position->y;
            copy->z -= position->z;
        }
        t2 = copy->z;
        t1 = target->x;
        *(((u8 *)actor) + 0x3cd) = Math_Atan2(t1, t2);
        tmp = zero;
        CpuSet((void *)(&tmp), context->clearF14, 0x05000002);
    } else
        actor->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", Axe_UpdateRouteDestinationPhase);
#endif
