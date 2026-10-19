#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#ifdef NONMATCHING
// fuzzy: 66.6% (agent draft, in progress; see notes/cloud-08122DF4.md)
u8 Golem_InvokeEnabledBehavior(struct SceneActorRegistryResetView *actor);
u8 SceneActorTarget_TrySelectNearestParticipant(struct SceneActorRegistryResetView *);
void sub_0807E334(struct SceneActorRegistryResetView *);
void sub_0807C72C(void *, u32, u32, u32);
s32 Callback_InvokeActor(void *, void *);

extern u8 *gUnk_02000580[4];
struct SavedControl {
    void *target;
    u8 state, phase, behavior, kind;
};

static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u8 TakeFlag2B1(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline u32 MaskFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, u32 kind, void *control)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetAnimation(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                                u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline s32 CellIndex(struct ContactPosition *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u8 *Properties(s32 index)
{
    u8 *p = TerrainModifiers_FindCellOverride(index, 1);
    if (p)
        return p + 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = index * 4 + 12;
        return map->cells + offset;
    }
}
static inline u32 CellHeight(u8 *p, struct ContactPosition *position)
{
    u32 shape = *p >> 4;
    u32 height = (*p & 15) << 8;
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
static inline u32 CellHeight2(struct ContactPosition *position, s32 cell)
{
    return CellHeight(Properties(cell), position);
}
static inline u32 TerrainHeight(struct ContactPosition *position)
{
    return CellHeight(Properties(CellIndex(position)), position);
}
static inline void LoadRoutePoint(struct ContactPosition *position, u8 *route)
{
    position->x = (*(u16 **)(route + 8))[0];
    position->z = (*(u16 **)(route + 8))[1];
    position->y = TerrainHeight(position);
}
static inline u8 RouteTo(struct SceneActorRegistryResetView *actor, void *routeObject,
                         struct ContactPosition *position, struct ContactPosition *target)
{
    if (!sub_0821F204(routeObject, position, target)) {
        u16 *flags = &actor->flags2D4;
        u32 mask = ~0x200;
        *flags &= mask;
        return 0;
    } else {
        s32 result = sub_0821F7A8(routeObject, position, target);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->motion, 8);
            return 1;
        } else {
            u16 *flags = &actor->flags2D4;
            u32 mask = ~0x200;
            *flags &= mask;
            return 0;
        }
    }
}
static inline u8 RouteToBit(struct SceneActorRegistryResetView *actor, void *routeObject,
                            struct ContactPosition *position, struct ContactPosition *target,
                            u32 bit)
{
    if (!sub_0821F204(routeObject, position, target)) {
        actor->flags2D4 &= ~bit;
        return 0;
    } else {
        s32 result = sub_0821F7A8(routeObject, position, target);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->motion, 8);
            return 1;
        } else {
            actor->flags2D4 &= ~bit;
            return 0;
        }
    }
}
static inline void SetCommand(struct SceneActorReactionCommand *command, u32 duration,
                              void *payload, u32 kind)
{
    command->duration = duration;
    command->payload = payload;
    command->first = 0;
    command->second = 0;
    command->kind = kind;
}
static inline void Turn(struct SceneActorRegistryResetView *actor, u8 direction)
{
    struct SceneActorRegistryResetView *self = actor;
    u8 *body = (u8 *)&actor->group;
    if (!MaskFlags(actor, 0x400) && !Test(actor->group.flags414, 4)) {
        u8 current = self->direction;
        s32 i = 0;
        u32 next;
        do {
            gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
            next = (gUnk_0203B400[gUnk_03005308] & 3) << 6;
            if (next != current)
                break;
        } while (++i <= 3);
        *(u8 *)(body + 0x204) = next;
        ((struct SavedControl *)(body + 0x8C))->target = self->target;
        ((struct SavedControl *)(body + 0x8C))->kind = self->field2B5;
        ((struct SavedControl *)(body + 0x8C))->state = self->state2A8;
        ((struct SavedControl *)(body + 0x8C))->phase = self->phase2A9;
        ((struct SavedControl *)(body + 0x8C))->behavior = self->behavior2AA;
        *(u8 *)(body + 0x205) = direction;
        SetControl(self, 0x1B, *(void **)(body + 0x21C));
        SetAnimation(self, 3, 1, 0);
        *(u8 *)(body + 0x206) = 6;
    }
}
static inline s32 PlayerIndex(u8 *partner)
{
    if (partner[8] <= 3)
        return partner[8];
    return -1;
}
static inline u32 Dazzled(s32 index)
{
    u8 *player = gUnk_02000580[index];
    u32 mask = 0x20000;
    return *(u32 *)(player + 0x20) & mask;
}
static inline u8 Approach(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRegistryResetView *self = actor;
    u8 *group = (u8 *)&self->group;
    s32 cell, index;
    struct ContactPosition center;
    LoadRoutePoint(actor->state5D4, (u8 *)&actor->route);
    if (Test(gUnk_030053F4, 0x4800)) {
        u8 *partner = *(u8 **)&actor->fieldDC;
        if (!partner)
            goto fail;
        index = PlayerIndex(partner);
        if (index < 0)
            goto fail;
        cell = *(u16 *)(*(u8 **)(gUnk_02000580[index] + 0x40) + 6);
    } else {
        cell = CellIndex((struct ContactPosition *)(*(u8 **)&actor->fieldDC + 0xC));
        index = 0;
    }
    if (cell == self->terrainCell || Dazzled(index)) {
        u8 direction;
        u16 busy = MaskFlags(self, 0x400);
        u16 blocked;
        if (busy)
            return 0;
        {
            u16 *flags = (u16 *)(group + 0x40);
            s32 mask = ~4;
            *flags &= mask;
        }
        blocked = MaskFlags(self, 0x80);
        if (blocked) {
            SetControl(self, 0x1A, *(void **)(group + 0x48));
            SetAnimation(self, 0, 0, 12);
        } else {
            SetControl(self, 1, self->entryTarget228);
            SetAnimation(self, 0, 0, 5);
            LoadRoutePoint(*(struct ContactPosition **)(group + 0x200), group + 0x74);
            (*(struct ContactPosition **)(group + 0x200))->y =
                CellHeight2(*(struct ContactPosition **)(group + 0x200),
                            CellIndex(*(struct ContactPosition **)(group + 0x200)));
            (*(struct ContactPosition **)(group + 0x200))->pad = 0;
        }
        direction = self->direction;
        Turn(actor, direction);
        sub_0807E334(actor);
        self->field2C2 = 0x20;
        return 0;
    }
    SceneActorTerrain_GetCenter(cell, &center);
    if (!RouteTo(actor, *(void **)(group + 0x1FC), &self->position, &center)) {
    fail:
        Callback_InvokeActor(self, *(void **)(group + 0x224));
        return 0;
    }
    return 1;
}
void sub_080CE188(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    if (TakeFlag2B1(actor))
        ;
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    if (actor->field2B0) {
        u16 blocked;
        SetAnimation(actor, 3, 1, 0);
        SetControl(actor, 14, SceneActorReaction_RestoreAfterDelay);
        ProximityVolume_TranslateBounds(*(void **)(group + 0x1FC), &actor->position);
        blocked = MaskFlags(actor, 0x80);
        if (blocked) {
            SetCommand((void *)(group + 0x8C), 30, *(void **)(group + 0x48), 12);
            if (Test(gUnk_030053F4, 0x4800)) {
                if (!SceneActorTarget_TrySelectNearestParticipant(actor))
                    return;
            } else
                actor->fieldDC = (u32)(gUnk_02000580[0] + 0x24);
            if (Approach(actor))
                actor->field2C2 = 0x20;
        } else {
            u32 bit = 0x200;
            u32 *flags = &actor->flags2CC;
            if (MaskFlags(actor, bit)) {
                struct SceneActorRegistryControl *control = (void *)(group + 0x4C);
                if (*flags & 0x200000) {
                    actor->flags2D0 &= ~0x1000000;
                    actor->flags2D4 &= ~bit;
                    control->mode = 0;
                    control->counter = 0;
                    control->timer = 0;
                }
                sub_0807C72C(actor, 0, 2, actor->category);
            }
            *flags |= 0x10;
            actor->field2C2 = 0x40;
            SetCommand((void *)(group + 0x8C), 60, actor->entryTarget228, 7);
            LoadRoutePoint(*(struct ContactPosition **)(group + 0x200), group + 0x74);
            (*(struct ContactPosition **)(group + 0x200))->pad = 1;
            if (!RouteToBit(actor, *(void **)(group + 0x1FC), &actor->position,
                            *(struct ContactPosition **)(group + 0x200), bit))
                Callback_InvokeActor(actor, *(void **)(group + 0x224));
        }
    } else
        actor->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080CE188);
#endif
