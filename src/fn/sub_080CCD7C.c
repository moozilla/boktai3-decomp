// MATCHING_CONCESSION: register-pinning
// confidence=low: existing Golem state interpretation retained.
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "player.h"
// confidence=low: Golem behavior table dispatches charge pursuit using stored heading, speed and stamina.
#ifdef NONMATCHING
// fuzzy: 80.4%
// Remaining mismatch: Charge/turn control resets and shared route-recovery pointer lifetimes still differ.
/* Golem: chase/charge behaviour (tentative). */
u8 Golem_InvokeEnabledBehavior(struct SceneActorRegistryResetView *actor);
void SceneActor_AccelerateForTerrain(struct SceneActorRegistryResetView *);
u8 Actor_StepTowardTargetAxis(struct SceneActorRegistryResetView *, void *, u32, u32, s32, u32);
s32 Math_Atan2(s32, s32);
s32 SceneActorTerrain_CompareNeighborHeight(s32, u32, u32);
void Skeleton_StepPosition(struct ContactPosition *, u32);
void sub_080C47DC(void *);
void sub_0807E334(struct SceneActorRegistryResetView *);
void SceneActorReaction_SlideBack(void);
void Golem_UpdateChargeStop(void);
void Golem_UpdateChargeRecovery(void);

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
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 CellHeight(struct ContactPosition *position, u32 cell)
{
    u32 shape, height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
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
    struct ContactPosition *position = &actor->body.position;
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        actor->flags2D4 &= ~0x200;
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->body.motion, 8);
            return 1;
        } else {
            actor->flags2D4 &= ~0x200;
            return 0;
        }
    }
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)

{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
/* Give up the charge: drain the stamina and switch to the recover control. */
static inline void Golem_Recover(struct GolemBody *body)
{
    body->stamina680 -= 30;
    body->direction691 = body->direction690;
    SetState(BODY, 0, 2, 7);

    {
        void *target = SceneActorReaction_SlideBack;
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
static inline void SetControlZ(struct SceneActorRegistryResetView *actor, u32 kind, void *control,
                               u8 byteZero, u32 zero)
{
    actor->field2B2 = 1;
    actor->field2B0 = byteZero;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = zero;
}
/* Turn towards direction690, spending stamina; jumps to the caller's resume/commit labels. */
#define GOLEM_TURN(body)                                                                           \
    {                                                                                              \
        zero = 0;                                                                                  \
        asm("" : "+r"(zero));                                                                      \
        body->direction691 = body->direction690;                                                   \
        body->direction = body->direction690;                                                      \
        body->stamina680 -= 10;                                                                    \
        byteZero = 0;                                                                              \
        if (body->stamina680 > 0)                                                                  \
            goto resume;                                                                           \
        {                                                                                          \
            void *target = Golem_UpdateChargeStop;                                                 \
            SetControlZ(BODY, 0x27, target, byteZero, zero);                                       \
        }                                                                                          \
        behavior = 3;                                                                              \
        goto commit;                                                                               \
    }
static inline void Golem_Retarget(struct SceneActorRegistryResetView *actor, struct GolemBody *body)
{
    s32 d = Wrap(actor->tracking.heading - body->direction68E);
    if ((d < 0 ? -d : d) <= 15 && actor->terrainCell != U16(actor, 0x2F6)) {
        body->target69C = body->home694;
        sub_080C47DC(body);
    }
}
static inline u32 Random(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u16 *)(index * 2 + RANDOM_TABLE_ADDR);
}
struct GolemRouteLink {
    u8 unknown0[8];
    s16 *point;
};
struct GolemRoutePoint {
    s16 x, z;
};
struct GolemRouteLink2 {
    u8 unknown0[8];
    struct GolemRoutePoint *point;
};
struct GolemFlags16 {
    u16 value;
};
struct GolemSavedControl {
    void *target;
    u8 state, phase, behavior, kind;
};
#define PTR(a, o) (*(void **)((u8 *)(a) + (o)))
static inline u32 Flag16(void *p, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return *(u16 *)((u8 *)p + off) & mask;
}
static inline u32 Flag32(void *p, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return *(u32 *)((u8 *)p + off) & mask;
}
static inline u8 TestBits16(u16 *p, u32 maskInput)
{
    u32 mask = maskInput;
    if (*p & mask)
        return 1;
    return 0;
}
static inline u8 TryMove4(struct SceneActorRegistryResetView *actor, void *route,
                          struct ContactPosition *position, struct ContactPosition *destination)
{
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        u32 mask = ~0x200;
        actor->flags2D4 = mask & actor->flags2D4;
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->body.motion, 8);
            return 1;
        } else {
            u32 mask = ~0x200;
            actor->flags2D4 = mask & actor->flags2D4;
            return 0;
        }
    }
}
static inline u32 RandomDirectionExcept(u32 current)
{
    s32 i;
    for (i = 0; i <= 3; i++) {
        u32 d;
        gRandomIndex = (gRandomIndex + 1) & 0x3FF;
        d = (gRandomTable[gRandomIndex] & 3) << 6;
        if (d != current)
            return d;
    }
    return current;
}
static inline void Golem_Wander2(struct SceneActorRegistryResetView *input, u8 direction)
{
    struct SceneActorRegistryResetView *actor = input;
    u8 *group = (u8 *)&actor->group.common;
    if (!Flag16(actor, 0x400, 0x2D4) && !TestBits16((u16 *)(group + 0x40), 4)) {
        group[0x204] = RandomDirectionExcept(actor->body.direction);
        ((struct GolemSavedControl *)(group + 0x8C))->target = actor->target;
        ((struct GolemSavedControl *)(group + 0x8C))->kind = actor->field2B5;
        ((struct GolemSavedControl *)(group + 0x8C))->state = actor->state2A8;
        ((struct GolemSavedControl *)(group + 0x8C))->phase = actor->phase2A9;
        ((struct GolemSavedControl *)(group + 0x8C))->behavior = actor->behavior2AA;
        group[0x205] = direction;
        {
            void *target = PTR(group, 0x21C);
            SetControl(actor, 0x1B, target);
        }
        SetState(actor, 3, 1, 0);
        group[0x206] = 6;
    }
}
static inline u8 Golem_Chase(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor;
    u8 *group;
    u32 cell;
    s32 slot;
    u32 *targetp;
    actor = input;
    group = (u8 *)&actor->group.common;
    targetp = &actor->tracking.target;
    {
        struct ContactPosition *dest = actor->group.destination;
        struct GolemRouteLink2 *link = (void *)&actor->group.homeRoute;
        dest->x = link->point->x;
        dest->z = link->point->z;
        dest->y = CellHeight(dest, CellIndex(dest));
    }
    if (Test(gEngineFlags, 0x4800)) {
        u8 *target = (u8 *)*targetp;
        if (!target)
            goto fail;
        slot = SlotIndex(target);
        if (slot < 0)
            goto fail;
        cell = *(u16 *)((u8 *)PTR(gPlayers[slot], 0x40) + 6);
    } else {
        cell = CellIndex((struct ContactPosition *)(*targetp + 0xC));
        slot = 0;
    }
    if (cell == actor->terrainCell || Flag32(gPlayers[slot], 0x20000, 0x20)) {
        u16 busy = Flag16(actor, 0x400, 0x2D4);
        u16 blocked;
        if (busy)
            return 0;
        {
            struct GolemFlags16 *flags = (void *)(group + 0x40);
            s32 mask = ~4;
            flags->value = mask & flags->value;
        }
        blocked = Flag16(actor, 0x80, 0x2D4);
        if (blocked) {
            void *target = PTR(group, 0x48);
            SetControl(actor, 0x1A, target);
            SetState(actor, 0, 0, 12);
        } else {
            {
                void *target = actor->entryTarget228;
                SetControl(actor, 1, target);
            }
            SetState(actor, 0, 0, 5);
            {
                struct ContactPosition **home = (struct ContactPosition **)(group + 0x200);
                {
                    struct ContactPosition *dest = *home;
                    struct GolemRouteLink2 *link = (struct GolemRouteLink2 *)(group + 0x74);
                    dest->x = link->point->x;
                    dest->z = link->point->z;
                    dest->y = CellHeight(dest, CellIndex(dest));
                }
                (*home)->y = CellHeight(*home, CellIndex(*home));
                (*home)->pad = 0;
            }
        }
        Golem_Wander2(input, actor->body.direction);
        sub_0807E334(input);
        actor->field2C2 = 0x20;
        return 0;
    }
    {
        struct ContactPosition center;
        SceneActorTerrain_GetCenter(cell, &center);
        if (!TryMove4(input, PTR(group, 0x1FC), &actor->body.position, &center)) {
        fail:
            Callback_InvokeActor(actor, PTR(group, 0x224));
            return 0;
        }
    }
    return 1;
}

static inline void SetCommand(struct SceneActorReactionCommand *command, void *payload, u32 kind,
                              u32 duration)
{
    command->duration = duration;
    command->payload = payload;
    command->first = 0;
    command->second = 0;
    command->kind = kind;
}
void Golem_UpdateChargePursuit(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    struct GolemBody *body = actor->context;
    struct ContactPosition diff;
    void *route;
    u32 behavior;
    u32 zero;
    u8 byteZero;
    if (TakeFlag2B1(actor))
        body->flag625 = 1;
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    if (body->flag625 == 1 && body->charge682 == 0) {
        SceneActorPlacement_PlayVariantEventSound((void *)actor, 4);
        body->flag625 = 0;
    }
    SceneActor_AccelerateForTerrain(actor);
    if (Test(gEngineFlags, 0x4800)) {
        u8 *partner = (u8 *)actor->tracking.target;
        s32 index;
        if (!partner)
            goto route;
        index = SlotIndex(partner);
        if (index < 0)
            goto route;
        if (IsDazzled(gPlayers[index])) {
            u32 mask = 0x80000;
            u8 held = Test(body->flags628, mask);
            if (!held) {
                Golem_Recover(body);
                return;
            }
        }
    } else if (IsDazzled(gPlayers[0])) {
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
            if (Actor_StepTowardTargetAxis(actor, route, 24, 24, body->charge682, 0)) {
                s32 distance;
                s32 d;
                body->target69C = body->home694;
                SceneActorTerrain_Subtract(&diff, &body->target69C, &actor->body.position);
                distance = diff.x * diff.x + diff.z * diff.z;
                {
                    u8 *facing = &body->direction68E;
                    body->flag625 = 1;
                    d = Wrap(actor->tracking.heading - *facing);
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
                                           &actor->tracking.target->position,
                                           &actor->body.position);
                if (diff.x * diff.x + diff.z * diff.z <= 0xFFFE && body->charge682 > 199) {
                    if (Test(gEngineFlags, 0x4800)) {
                        u8 *partner = (u8 *)actor->tracking.target;
                        s32 index;
                        if (!partner)
                            goto recover;
                        index = SlotIndex(partner);
                        if (index < 0)
                            goto recover;
                        if (IsDazzled(gPlayers[index]))
                            goto recover;
                        body->charge688 = 400;
                    } else {
                        if (IsDazzled(gPlayers[0]))
                            goto recover;
                        body->charge688 = 400;
                    }
                }
            recover:
                Golem_Recover(body);
                return;
            }
            Golem_Retarget(actor, body);
            if (Golem_TurnGap(body) > 31)
                GOLEM_TURN(body);
        } else {
            if (Actor_StepTowardTargetAxis(actor, route, 24, 24, body->charge682, 0)) {
                body->target69C = body->home694;
                sub_080C47DC(body);
            } else
                Golem_Retarget(actor, body);
            if (Golem_TurnGap(body) > 31) {
                GOLEM_TURN(body);
            resume:
                body->target69C = *(struct ContactPosition *)(body->route5D0 + 0x1C);
                {
                    void *target = Golem_UpdateChargeRecovery;
                    SetControlZ(BODY, 0x26, target, byteZero, zero);
                }
                behavior = 2;
            commit:
                BODY->behavior2AA = behavior;
                BODY->counter2AC = zero;
                BODY->field2B1 = 1;
                return;
            }
        }
    }
    if (body->stamina680 > 0)
        goto advance;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    SetState(actor, 3, 1, 0);
    {
        void *target = SceneActorReaction_RestoreAfterDelay;
        SetControl(actor, 0xE, target);
    }
    {
        u16 *flags = &actor->flags2D4;
        s32 mask = ~0x100;
        *flags = mask & *flags;
    }
    if (actor->flags2D4 & 0x80) {
        SetCommand((void *)(group + 0x8C), *(void **)(group + 0x48), 12, 30);
        if (!Golem_Chase(actor))
            return;
        actor->field2C2 = 0x20;
        goto advance;
    } else {
        actor->flags2CC |= 0x10;
        actor->field2C2 = 0x40;
        SetCommand((void *)(group + 0x8C), actor->entryTarget228, 7, 60);
        {
            struct ContactPosition *dest = *(struct ContactPosition **)(group + 0x200);
            struct GolemRouteLink *link = (void *)(group + 0x74);
            dest->x = link->point[0];
            dest->z = link->point[1];
            dest->y = CellHeight(dest, CellIndex(dest));
        }
        (*(struct ContactPosition **)(group + 0x200))->pad = 1;
        if (!TryMove4(actor, PTR(group, 0x1FC), &actor->body.position,
                      *(struct ContactPosition **)(group + 0x200))) {
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
            return;
        }
    }
advance:
    body->counter686++;
    actor->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", Golem_UpdateChargePursuit);
#endif
