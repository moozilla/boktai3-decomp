#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "player.h"
#ifdef NONMATCHING
// fuzzy: 62.4% (agent draft, in progress; see notes/cloud-08122DF4.md)
/* Golem: chase/charge behaviour (tentative). */
u8 Golem_InvokeEnabledBehavior(struct SceneActorRegistryResetView *actor);
void SceneActor_AccelerateForTerrain(struct SceneActorRegistryResetView *);
u8 sub_080C44E4(struct SceneActorRegistryResetView *, void *, u32, u32, s32, u32);
s32 Math_Atan2(s32, s32);
s32 SceneActorTerrain_CompareNeighborHeight(s32, u32, u32);
void Skeleton_StepPosition(struct ContactPosition *, u32);
void sub_080C47DC(void *);
void sub_0807E334(struct SceneActorRegistryResetView *);
void sub_080C79A8(void);
void sub_080C68F0(void);
void sub_080C6A68(void);

#define S16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define U16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define POS(p, o) (*(struct ContactPosition *)((u8 *)(p) + (o)))
#define BODY      ((struct SceneActorRegistryResetView *)body)
struct GolemBody {
    u8 unknown0[0x59];
    u8 direction;
    u8 unknown5A[0x5D0 - 0x5A];
    u8 *route5D0;
    u8 unknown5D4[0x625 - 0x5D4];
    u8 flag625;
    u8 unknown626[2];
    u32 flags628;
    u8 unknown62C[0x680 - 0x62C];
    s16 stamina680, charge682;
    u8 unknown684[2];
    u16 counter686;
    u16 charge688;
    u8 unknown68A[4];
    u8 direction68E, unknown68F, direction690, direction691;
    u8 unknown692[2];
    struct ContactPosition home694, target69C;
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
static inline u32 CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u32 CellHeight(struct ContactPosition *position, u32 cell)
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
static inline s32 SlotIndex(u8 *partner)
{
    if (partner[8] <= 3)
        return partner[8];
    return -1;
}
static inline u32 IsDazzled(struct Player *player)
{
    u32 mask = 0x20000;
    return player->statusFlags & mask;
}
static inline s32 Wrap(s32 d)
{
    if (d > 128)
        d -= 256;
    else if (d < -128)
        d += 256;
    return d;
}
static inline u8 TryMove(struct SceneActorRegistryResetView *actor, u8 *group,
                         struct ContactPosition *destination)
{
    void *route = *(void **)(group + 0x1FC);
    struct ContactPosition *position = &actor->position;
    s32 result;
    if (!sub_0821F204(route, position, destination)) {
        actor->flags2D4 &= ~0x200;
        return 0;
    } else {
        result = sub_0821F7A8(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->motion, 8);
            return 1;
        } else {
            actor->flags2D4 &= ~0x200;
            return 0;
        }
    }
}
/* Give up the charge: drain the stamina and switch to the recover control. */
static inline void Golem_Recover(struct GolemBody *body)
{
    body->stamina680 -= 30;
    body->direction691 = body->direction690;
    {
        u32 phase = 2, behavior = 7;
        BODY->state2A8 = 0;
        BODY->phase2A9 = phase;
        BODY->behavior2AA = behavior;
        BODY->counter2AC = 0;
        BODY->field2B1 = 1;
    }
    {
        void *target = sub_080C79A8;
        SetControl(BODY, 0x2B, target);
    }
}
static inline s32 Golem_TurnGap(struct GolemBody *body)
{
    s32 d = Wrap(body->direction690 - body->direction68E);
    if (d < 0)
        d = -d;
    return d;
}
/* Turn towards direction690, spending stamina; jumps to the caller's resume/commit labels. */
#define GOLEM_TURN(body)                                                                           \
    {                                                                                              \
        body->direction691 = body->direction690;                                                   \
        body->direction = body->direction690;                                                      \
        body->stamina680 -= 10;                                                                    \
        if (body->stamina680 > 0)                                                                  \
            goto resume;                                                                           \
        {                                                                                          \
            void *target = sub_080C68F0;                                                           \
            SetControl(BODY, 0x27, target);                                                        \
        }                                                                                          \
        behavior = 3;                                                                              \
        goto commit;                                                                               \
    }
static inline void Golem_Retarget(struct SceneActorRegistryResetView *actor, struct GolemBody *body)
{
    s32 d = Wrap(actor->fieldE9 - body->direction68E);
    if ((d < 0 ? -d : d) <= 15 && actor->terrainCell != U16(actor, 0x2F6)) {
        body->target69C = body->home694;
        sub_080C47DC(body);
    }
}
static inline u32 Random(void)
{
    u32 index = (gUnk_03005308 + 1) & 1023;
    gUnk_03005308 = index;
    return *(const u16 *)(index * 2 + RANDOM_TABLE_ADDR);
}
struct GolemRouteLink {
    u8 unknown0[8];
    s16 *point;
};
static inline void Golem_Wander(struct SceneActorRegistryResetView *self, u8 *base)
{
    u8 direction = self->direction;
    struct SceneActorRegistryResetView *actor = self;
    u8 *group = base;
    if (!(actor->flags2D4 & 0x400) && !Test(actor->group.flags414, 4)) {
        u32 current = actor->direction;
        s32 i = 0;
        u32 next;
        while (1) {
            u32 offset;
            gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
            offset = gUnk_03005308 * 2;
            next = (*(u16 *)(offset + (u32)gUnk_0203B400) & 3) << 6;
            if (next == current) {
                i++;
                if (i <= 3)
                    continue;
                next = current;
            }
            group[0x204] = next;
            {
                struct {
                    void *control;
                    u8 state, phase, behavior, kind;
                } *saved = (void *)(group + 0x8C);
                saved->control = actor->target;
                saved->kind = actor->field2B5;
                saved->state = actor->state2A8;
                saved->phase = actor->phase2A9;
                saved->behavior = actor->behavior2AA;
            }
            group[0x205] = direction;
            {
                void *target = *(void **)(group + 0x21C);
                SetControl(actor, 0x1B, target);
            }
            SceneActorReaction_ResetAnimation(actor, 1);
            group[0x206] = 6;
            break;
        }
    }
    sub_0807E334(actor);
    actor->field2C2 = 0x20;
}
static inline u8 Golem_Pursue(struct SceneActorRegistryResetView *self)
{
    struct SceneActorRegistryResetView *actor = self;
    u8 *group = (u8 *)&actor->group;
    struct ContactPosition center;
    struct ContactPosition *dest = actor->state5D4;
    struct GolemRouteLink *link = (void *)&actor->route;
    u32 cell;
    s32 index;
    dest->x = link->point[0];
    dest->z = link->point[1];
    dest->y = CellHeight(dest, CellIndex(dest));
    if (Test(gUnk_030053F4, 0x4800)) {
        u8 *partner = (u8 *)actor->fieldDC;
        if (!partner)
            goto fail;
        index = SlotIndex(partner);
        if (index < 0)
            goto fail;
        cell = *(u16 *)(*(u8 **)((u8 *)gUnk_02000580[index] + 0x40) + 6);
    } else {
        cell = CellIndex((struct ContactPosition *)((u8 *)actor->fieldDC + 0xC));
        index = 0;
    }
    if (cell == actor->terrainCell || IsDazzled(gUnk_02000580[index])) {
        if (actor->flags2D4 & 0x400)
            return 0;
        *(u16 *)(group + 0x40) &= ~4;
        if (actor->flags2D4 & 0x80) {
            void *target = *(void **)(group + 0x48);
            SetControl(actor, 0x1A, target);
            {
                u32 behavior = 12;
                actor->state2A8 = 0;
                actor->phase2A9 = 0;
                actor->behavior2AA = behavior;
                actor->counter2AC = 0;
                actor->field2B1 = 1;
            }
        } else {
            struct ContactPosition *p;
            void *target = actor->entryTarget228;
            SetControl(actor, 1, target);
            {
                u32 behavior = 5;
                actor->state2A8 = 0;
                actor->phase2A9 = 0;
                actor->behavior2AA = behavior;
                actor->counter2AC = 0;
                actor->field2B1 = 1;
            }
            p = *(struct ContactPosition **)(group + 0x200);
            p->x = ((struct GolemRouteLink *)(group + 0x74))->point[0];
            p->z = ((struct GolemRouteLink *)(group + 0x74))->point[1];
            p->y = CellHeight(p, CellIndex(p));
            (*(struct ContactPosition **)(group + 0x200))->y =
                CellHeight(*(struct ContactPosition **)(group + 0x200),
                           CellIndex(*(struct ContactPosition **)(group + 0x200)));
            (*(struct ContactPosition **)(group + 0x200))->pad = 0;
        }
        Golem_Wander(actor, group);
        return 0;
    }
    SceneActorTerrain_GetCenter(cell, &center);
    if (TryMove(actor, group, &center))
        return 1;
fail:
    Callback_InvokeActor(actor, *(void **)(group + 0x224));
    return 0;
}
void sub_080CCD7C(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    struct GolemBody *body = actor->context;
    struct ContactPosition diff;
    void *route;
    u32 behavior;
    if (TakeFlag2B1(actor))
        body->flag625 = 1;
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    if (body->flag625 == 1 && body->charge682 == 0) {
        SceneActorPlacement_PlayVariantEventSound((void *)actor, 4);
        body->flag625 = 0;
    }
    SceneActor_AccelerateForTerrain(actor);
    if (Test(gUnk_030053F4, 0x4800)) {
        u8 *partner = (u8 *)actor->fieldDC;
        s32 index;
        if (!partner)
            goto route;
        index = SlotIndex(partner);
        if (index < 0)
            goto route;
        if (IsDazzled(gUnk_02000580[index])) {
            u32 mask = 0x80000;
            u8 held = Test(body->flags628, mask);
            if (!held) {
                Golem_Recover(body);
                return;
            }
        }
    } else if (IsDazzled(gUnk_02000580[0])) {
        u32 mask = 0x80000;
        u8 held = Test(body->flags628, mask);
        if (!held) {
            Golem_Recover(body);
            return;
        }
    }
route:
    route = *(void **)(group + 0x1FC);
    {
        u16 flag = *(u16 *)route & 4;
        if (flag) {
            if (sub_080C44E4(actor, route, 24, 24, body->charge682, 0)) {
                s32 distance;
                s32 d;
                body->target69C = body->home694;
                SceneActorTerrain_Subtract(&diff, &body->target69C, &actor->position);
                distance = diff.x * diff.x + diff.z * diff.z;
                {
                    u8 *facing = &body->direction68E;
                    body->flag625 = 1;
                    d = Wrap(actor->fieldE9 - *facing);
                }
                if (d < 0)
                    d = -d;
                if (d <= 15 && distance <= 0xFFFE && body->charge682 > 199)
                    goto recover;
                {
                    u32 facing = body->direction68E;
                    s32 a = Wrap(Math_Atan2(diff.x, diff.z) - facing);
                    a = a < 0 ? -a : a;
                    if (a <= 15) {
                        u32 cell = CellIndex(&body->target69C);
                        u32 height = *SceneActorTerrain_GetCell(cell) & 15;
                        u32 direction = ((body->direction68E + 0x20) & 0xFF) >> 6;
                        if (!SceneActorTerrain_CompareNeighborHeight(cell, height, direction))
                            Skeleton_StepPosition(&body->target69C, direction);
                        sub_080C47DC(body);
                    } else if ((u32)(a - 0x34) <= 0x18)
                        sub_080C47DC(body);
                }
                if (Golem_TurnGap(body) > 31)
                    GOLEM_TURN(body);
                SceneActorTerrain_Subtract(&diff,
                                           (struct ContactPosition *)((u8 *)actor->fieldDC + 0xC),
                                           &actor->position);
                if (diff.x * diff.x + diff.z * diff.z <= 0xFFFE && body->charge682 > 199) {
                    if (Test(gUnk_030053F4, 0x4800)) {
                        u8 *partner = (u8 *)actor->fieldDC;
                        s32 index;
                        if (!partner)
                            goto recover;
                        index = SlotIndex(partner);
                        if (index < 0)
                            goto recover;
                        if (IsDazzled(gUnk_02000580[index]))
                            goto recover;
                    } else if (IsDazzled(gUnk_02000580[0]))
                        goto recover;
                    body->charge688 = 400;
                }
            recover:
                Golem_Recover(body);
                return;
            }
            Golem_Retarget(actor, body);
            if (Golem_TurnGap(body) > 31)
                GOLEM_TURN(body);
        } else {
            if (sub_080C44E4(actor, route, 24, 24, body->charge682, 0)) {
                body->target69C = body->home694;
                sub_080C47DC(body);
            } else
                Golem_Retarget(actor, body);
            if (Golem_TurnGap(body) > 31) {
                GOLEM_TURN(body);
            resume:
                body->target69C = *(struct ContactPosition *)(body->route5D0 + 0x1C);
                {
                    void *target = sub_080C6A68;
                    SetControl(BODY, 0x26, target);
                }
                behavior = 2;
            commit:
                BODY->behavior2AA = behavior;
                BODY->counter2AC = 0;
                BODY->field2B1 = 1;
                return;
            }
        }
    }
    if (body->stamina680 > 0)
        goto advance;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    SceneActorReaction_ResetAnimation(actor, 1);
    {
        void *target = SceneActorReaction_RestoreAfterDelay;
        SetControl(actor, 0xE, target);
    }
    actor->flags2D4 &= ~0x100;
    if (actor->flags2D4 & 0x80) {
        {
            struct SceneActorReactionCommand *command = (void *)(group + 0x8C);
            command->duration = 30;
            command->payload = *(void **)(group + 0x48);
            command->first = 0;
            command->second = 0;
            command->kind = 12;
        }
        if (!Golem_Pursue(actor))
            return;
        actor->field2C2 = 0x20;
        goto advance;
    } else {
        actor->flags2CC |= 0x10;
        actor->field2C2 = 0x40;
        {
            struct SceneActorReactionCommand *command = (void *)(group + 0x8C);
            command->duration = 60;
            command->payload = actor->entryTarget228;
            command->first = 0;
            command->second = 0;
            command->kind = 7;
        }
        {
            struct ContactPosition *dest = *(struct ContactPosition **)(group + 0x200);
            struct GolemRouteLink *link = (void *)(group + 0x74);
            dest->x = link->point[0];
            dest->z = link->point[1];
            dest->y = CellHeight(dest, CellIndex(dest));
        }
        (*(struct ContactPosition **)(group + 0x200))->pad = 1;
        if (!TryMove(actor, group, *(struct ContactPosition **)(group + 0x200))) {
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
            return;
        }
    }
advance:
    body->counter686++;
    actor->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080CCD7C);
#endif
