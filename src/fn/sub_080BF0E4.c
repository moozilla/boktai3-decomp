#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#ifdef NONMATCHING
// fuzzy: 72.5% (agent draft, paused; see notes/cloud-08122DF4.md)
/* Skeleton: on entry play the category's directional rise animation (0x63/0x65/0x67) or hand
 * over to SceneActorReaction_PlayAnimationBUntilComplete; once done, return to the route
 * point, then either resume the route (target cell reached) or walk toward the target cell
 * (tentative). */
void SceneActorReaction_PlayAnimationBUntilComplete(void);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorBehavior_CallActionScript(void *);
void sub_0807E334(void *);

struct SkeletonRoutePoint {
    s16 x, z;
};
struct SkeletonRouteLink {
    u8 unknown0[8];
    struct SkeletonRoutePoint *point;
};
struct SkeletonPlayerCell {
    u8 unknown0[6];
    u16 cell;
};
struct SkeletonPlayer {
    u8 unknown0[0x20];
    u32 flags;
    u8 unknown24[0x1C];
    struct SkeletonPlayerCell *cell;
};
extern struct SkeletonPlayer *gPlayers[4];

struct SkeletonSavedControl {
    void *target;
    u8 state, phase, behavior, kind;
};
struct SkeletonGroup {
    u8 unknown0[0x40];
    u16 flags414, field416, field418;
    u8 unknown46[2];
    void *entryControl41C;
    u8 unknown4C[0x28];
    struct SkeletonRouteLink link;
    u8 unknown80[0xC];
    struct SkeletonSavedControl saved;
    u8 unknown94[0x168];
    void *routeObject;
    struct ContactPosition *destination;
    u8 direction, savedDirection, timer, unknown207;
    u8 unknown208[0x14];
    void *control;
    u8 unknown220[4];
    void *failure;
};

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
static inline void SetControlZ(struct SceneActorRegistryResetView *actor, u32 kind, void *control,
                               u32 zero)
{
    actor->field2B2 = 1;
    actor->field2B0 = zero;
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
static inline void SetCtl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
#define PLAY_DIRECTIONAL(value, done)                                                              \
    do {                                                                                           \
        struct SceneActorProfileEntry *entry;                                                      \
        u8 directionVariant, mirror;                                                               \
        u32 selection;                                                                             \
        u8 *mirrorPointer;                                                                         \
        u8 *variantPointer = &directionVariant;                                                    \
        u32 flags, variant;                                                                        \
        u8 entryFlags, mirrorValue;                                                                \
        u8 *variantReload;                                                                         \
        u32 force;                                                                                 \
        u8 *restart;                                                                               \
        {                                                                                          \
            u8 *mirrorAddress = &mirror;                                                           \
            s32 quadrant = (((s32)actor->direction + 0x60) >> 6) & 3;                              \
            mirrorPointer = mirrorAddress;                                                         \
            if (quadrant > 1) {                                                                    \
                *mirrorPointer = 1;                                                                \
                *variantPointer = 3 - quadrant;                                                    \
            } else {                                                                               \
                *mirrorPointer = 0;                                                                \
                *variantPointer = quadrant;                                                        \
            }                                                                                      \
        }                                                                                          \
        entry =                                                                                    \
            (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 0);   \
        do {                                                                                       \
            variantReload = &directionVariant;                                                     \
            mirrorValue = *mirrorPointer;                                                          \
            entryFlags = entry->flags;                                                             \
        } while (0);                                                                               \
        selection = value;                                                                         \
        variant = *variantReload;                                                                  \
        flags = mirrorValue | entryFlags;                                                          \
        if (flags & 0x10) {                                                                        \
            force = 0x10;                                                                          \
            flags &= ~0x10;                                                                        \
        } else                                                                                     \
            force = 0;                                                                             \
        if (!actor->drawableKind) {                                                                \
            struct SceneActorAnimationSequenceDrawable *drawable;                                  \
            actor->unknown2B7 = selection;                                                         \
            drawable = actor->drawable;                                                            \
            Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,       \
                             (u8)variant, (u8)flags);                                              \
            restart = &actor->field2B3;                                                            \
        } else {                                                                                   \
            struct SceneActorAnimationTile *tile =                                                 \
                &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;                \
            u8 *savedSelection;                                                                    \
                                                                                                   \
            selection += variant;                                                                  \
            savedSelection = &actor->unknown2B7;                                                   \
            restart = &actor->field2B3;                                                            \
            if (!force && *savedSelection == selection && !*restart) {                             \
                if (flags == 1 && (tile->flags & 4))                                               \
                    goto done;                                                                     \
                if (flags == 0 && !(tile->flags & 4))                                              \
                    goto done;                                                                     \
            }                                                                                      \
            *savedSelection = selection;                                                           \
            {                                                                                      \
                u32 mask = 4;                                                                      \
                u32 mode;                                                                          \
                if (flags & mask)                                                                  \
                    mode = 6;                                                                      \
                else                                                                               \
                    mode = 4;                                                                      \
                Anim_SetKeyframes(                                                                 \
                    &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,            \
                    actor->drawable, (u16)selection, 2, (u16)mode);                                \
                flags &= 1;                                                                        \
                if (flags)                                                                         \
                    tile->flags |= mask;                                                           \
                else                                                                               \
                    tile->flags &= ~4;                                                             \
            }                                                                                      \
        }                                                                                          \
        *restart = 0;                                                                              \
    } while (0)

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
static inline u32 Flags2D4(struct SceneActorRegistryResetView *actor, u32 maskInput)
{
    u32 mask = maskInput;
    u16 *flags = &actor->flags2D4;
    return *flags & mask;
}
static inline void Or16(u16 *p, u32 maskInput)
{
    u32 mask = maskInput;
    mask |= *p;
    *p = mask;
}
static inline u8 IsScenePaused(void)
{
    if (gEngineFlags & 0x4800)
        return 1;
    return 0;
}
static inline u8 TestFlag16(u16 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}
static inline s32 PlayerSlot(u8 *target)
{
    s32 slot = -1;
    if (target[8] <= 3)
        slot = target[8];
    return slot;
}
static inline u8 MoveTo(struct SceneActorRegistryResetView *actor, struct SkeletonGroup *group,
                        struct ContactPosition *destination)
{
    void *route = group->routeObject;
    struct ContactPosition *position = &actor->position;
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        actor->flags2D4 &= ~0x200;
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
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
static inline void Turn(struct SceneActorRegistryResetView *actor, struct SkeletonGroup *group,
                        u8 newDirection, u8 direction)
{
    struct SkeletonSavedControl *saved;
    group->direction = newDirection;
    saved = &group->saved;
    saved->target = actor->target;
    saved->kind = actor->field2B5;
    saved->state = actor->state2A8;
    saved->phase = actor->phase2A9;
    saved->behavior = actor->behavior2AA;
    group->savedDirection = direction;
    SetCtl(actor, group->control, 0x1B);
    SetState(actor, 3, 1, 0);
    group->timer = 6;
}
static inline u32 PickDirection(u32 current)
{
    s32 i;
    for (i = 0; i <= 3; i++) {
        u32 next;
        gRandomIndex = (gRandomIndex + 1) & 0x3FF;
        next = (gRandomTable[gRandomIndex] & 3) << 6;
        if (next != current)
            return next;
    }
    return current;
}
static inline void Resume(void *argument, struct SkeletonGroup *group, u8 direction)
{
    struct SceneActorRegistryResetView *actor = argument;
    if (!Flags2D4(actor, 0x400) && !TestFlag16(&actor->group.flags414, 4)) {
        Turn(actor, group, PickDirection(actor->direction), direction);
    }
}
static inline u32 Flags32(u32 *p, u32 maskInput)
{
    u32 mask = maskInput;
    return *p & mask;
}
static inline void And16(u16 *p, u32 maskInput)
{
    u32 mask = maskInput;
    mask &= *p;
    *p = mask;
}
static inline void SetCtlZ(struct SceneActorRegistryResetView *actor, void *cb, u32 k, u8 zero)
{
    void *callback = cb;
    u32 kind = k;
    actor->field2B2 = 1;
    actor->field2B0 = zero;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}
static inline void ReturnHome(struct SceneActorRegistryResetView *outer)
{
    struct SceneActorRegistryResetView *actor = (void *)outer;
    struct SkeletonGroup *group = (void *)&actor->group;
    struct ContactPosition *destination;
    struct SkeletonRouteLink *link;
    u8 **target = (u8 **)&actor->fieldDC;
    u32 cell;
    s32 slot;
    destination = group->destination;
    link = &group->link;
    destination->x = link->point->x;
    destination->z = link->point->z;
    destination->y = CellHeight(destination, CellIndex(destination));
    if (IsScenePaused()) {
        if (!*target)
            goto fail;
        slot = PlayerSlot(*target);
        if (slot < 0)
            goto fail;
        cell = gPlayers[slot]->cell->cell;
    } else {
        cell = CellIndex((struct ContactPosition *)(*target + 12));
        slot = 0;
    }
    if (cell == actor->terrainCell || Flags32(&gPlayers[slot]->flags, 0x20000)) {
        u16 busy = Flags2D4(actor, 0x400);
        u8 zero;
        if (busy)
            return;
        And16(&group->flags414, ~4);
        zero = 0;
        {
            u16 entry = Flags2D4(actor, 0x80);
            if (entry) {
                SetCtlZ(actor, group->entryControl41C, 0x1A, zero);
                {
                    u32 behavior = 12;
                    actor->state2A8 = zero;
                    actor->phase2A9 = zero;
                    actor->behavior2AA = behavior;
                    actor->counter2AC = 0;
                    actor->field2B1 = 1;
                }
            } else {
                SetCtlZ(actor, actor->entryTarget228, 1, zero);
                {
                    u32 behavior = 5;
                    actor->state2A8 = zero;
                    actor->phase2A9 = zero;
                    actor->behavior2AA = behavior;
                    actor->counter2AC = 0;
                    actor->field2B1 = 1;
                }
                destination = group->destination;
                link = &group->link;
                destination->x = link->point->x;
                destination->z = link->point->z;
                destination->y = CellHeight(destination, CellIndex(destination));
                group->destination->y =
                    CellHeight(group->destination, CellIndex(group->destination));
                group->destination->pad = 0;
            }
        }
        Resume(outer, group, actor->direction);
        sub_0807E334(outer);
        actor->field2C2 = 0x20;
        return;
    } else {
        struct ContactPosition center;
        SceneActorTerrain_GetCenter(cell, &center);
        if (MoveTo(outer, group, &center))
            return;
    }
fail:
    Callback_InvokeActor(actor, group->failure);
}

void sub_080BF0E4(struct SceneActorRegistryResetView *actor)
{
    struct SkeletonGroup *group = (void *)&actor->group;
    if (TakeFlag2B1(actor)) {
        u8 *category;
        u16 *flags;
        u32 zero;
        actor->direction = actor->fieldE9;
        zero = 0;
        category = &actor->category;
        if (*category == 5) {
            u32 mask = 0x200;
            flags = &actor->flags2D4;
            if ((*flags & mask) || actor->unknown2B7 == 0x63) {
                PLAY_DIRECTIONAL(0x63, done5);
            done5:
                actor->field2B0 = 1;
            } else
                SetCtlZ(actor, SceneActorReaction_PlayAnimationBUntilComplete, 10, zero);
        } else if (*category == 6) {
            u32 mask = 0x200;
            flags = &actor->flags2D4;
            if ((*flags & mask) || actor->unknown2B7 == 0x65) {
                PLAY_DIRECTIONAL(0x65, done6);
            done6:
                actor->field2B0 = 1;
            } else
                SetCtlZ(actor, SceneActorReaction_PlayAnimationBUntilComplete, 10, zero);
        } else {
            flags = &actor->flags2D4;
            if (*category == 7) {
                u32 mask = 0x200;
                u16 blocked = *flags & mask;
                if (blocked || actor->unknown2B7 == 0x67) {
                    PLAY_DIRECTIONAL(0x67, done7);
                done7:
                    actor->field2B0 = 1;
                } else
                    SetCtlZ(actor, SceneActorReaction_PlayAnimationBUntilComplete, 10, zero);
            }
        }
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 0, *category);
        SceneActorBehavior_CallActionScript(actor);
        Or16(&actor->group.flags414, 4);
        Or16(flags, 0x80);
        actor->group.field418 = actor->group.field416;
    }
    if (actor->field2B0) {
        {
            u32 behavior = 12;
            actor->state2A8 = 0;
            actor->phase2A9 = 0;
            actor->behavior2AA = behavior;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
        }
        SetCtl(actor, group->entryControl41C, 0x1A);
        ReturnHome(actor);
    } else
        actor->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080BF0E4);
#endif
