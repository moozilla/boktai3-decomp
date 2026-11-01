#include "scene_actor_part_burst.h"
#include "scene_actor_palette.h"
#include "engine.h"
#include "ram.h"
#include "scene_actor_placement.h"
#include "game_state.h"
#ifdef NONMATCHING
// fuzzy: 88.4%
// The action and motion flag reads span the regional-deactivation and palette branches.
void Heap_Free(void *);

static inline u8 Removed(struct SceneActorRegistryActor *p)
{
    u32 mask = 0x1000;
    if (mask & p->flags2D6)
        return 1;
    return 0;
}
static inline u8 Paused(void)
{
    if (gEngineFlags & 0x4800)
        return 1;
    return 0;
}
static inline u8 CurrentRegion(struct SceneActorRegistryActor *p)
{
    u16 region = p->regionId;
    if (gGameState->region == region)
        return 1;
    return 0;
}
static inline u8 TransitionEnabled(struct SceneActorRegistryManager *p)
{
    if (p->flags & 0x2000)
        return 1;
    return 0;
}
static inline u8 TransitionComplete(struct SceneActorRegistryManager *p)
{
    u32 mask = 0x20000;
    if (mask & p->flags)
        return 1;
    return 0;
}
static inline u8 Stopped(void)
{
    u16 pending = gPlayerActionPending;
    u32 value = 0;
    if (pending == 1)
        value = 1;
    return value;
}
static inline u32 NextParticleSample(const u16 *table)
{
    u16 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}
static inline void UpdateActors_SpawnContactParts(struct SceneActorRegistryActor *actor)
{
    union SceneActorRegistryPackedPosition velocity, bounds, position;
    s32 i;
    for (i = 0; i < 3; i++) {
        union RandomTableView table = {gRandomTable};
        velocity.packed.x = (NextParticleSample(table.samples) & 15) - 7;
        velocity.packed.y = ((s8)NextParticleSample(table.samples) & 31) - 16;
        velocity.packed.z = ((s8)NextParticleSample(table.samples) & 15) - 7;
        bounds.packed.x = 5;
        bounds.packed.y = 10;
        bounds.packed.z = 5;
        position.packed.x = actor->effectOrigin->x;
        position.packed.y = actor->effectOrigin->y + 128;
        position.packed.z = actor->effectOrigin->z;
        EffectGroup_SpawnPart((void *)actor->effectConfig, 3, &position.position,
                              &velocity.position, &bounds.position, 40, 40);
    }
    actor->fieldAA = 0;
    actor->field2B8 = 0;
}

static inline u32 ReadFlags2CC(struct SceneActorRegistryActor *p, u32 mask)
{
    return p->flags2CC & mask;
}
struct UpdateActorsTile {
    u8 unknown0[8];
    u32 flags;
    u8 unknown0C[0x2E];
    u16 paletteOffset;
};
static inline void ClearWordFlags(u32 *flags, u32 bits)
{
    u32 mask = ~bits;
    *flags &= mask;
}
static inline void ClearTileFlags(struct UpdateActorsTile *tile, u32 bits)
{
    u32 keep = ~bits;
    tile->flags &= keep;
}
static inline void ClearDrawableFlags(struct SceneActorRegistryActor *actor, u32 bits)
{
    if (actor->drawableKind == 0)
        ClearWordFlags(&((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->flags,
                       bits);
    else {
        struct UpdateActorsTile *tile = (void *)((u8 *)actor->drawable + 0x20);
        ClearTileFlags(tile, bits);
    }
}
static inline void ClearField2B8(struct SceneActorRegistryActor *p)
{
    p->field2B8 = 0;
}
static inline void SetFlags2CC(struct SceneActorRegistryActor *p, u32 bits)
{
    p->flags2CC |= bits;
}
static inline u8 ActorCategory(struct SceneActorRegistryActor *p)
{
    return p->category;
}

/* The motion and state words are adjoining members of the same registry actor. */
static inline u32 ReadStateFlags(const u16 *motion, u32 mask)
{
    u32 motionOffset = (u32) & ((struct SceneActorRegistryActor *)0)->flags2D4;
    const struct SceneActorRegistryActor *actor = (const void *)((const u8 *)motion - motionOffset);
    return actor->flags2D0 & mask;
}
static inline u8 CanUpdate(struct SceneActorRegistryActor *p, u16 *flags)
{
    if (p) {
        if ((*flags & 0x40) || ReadStateFlags(flags, 0x20000) || p->health <= 0)
            return 0;
    }
    return 1;
}
static inline u32 DefaultPalette(struct SceneActorRegistryActor *actor)
{
    u16 *base = &actor->basePaletteBank;
    u16 *offset = &actor->paletteOffset;
    return *base + *offset;
}

static inline u32 ReadMotionFlags(u16 *motion, u32 mask)
{
    return *motion & mask;
}
static inline u8 MotionFlagSet(const u16 *flags, u32 mask)
{
    u8 found = 0;
    if (*flags & mask)
        found = 1;
    return found;
}
static inline u8 MissingObject(struct SceneActorRegistryActor *p)
{
    if (!p->object)
        return 1;
    return 0;
}

// confidence=low: registry callback traversal, regional deactivation and palette transitions.
void SceneActorRegistry_UpdateActors(struct SceneActorRegistryManager *manager)
{
    struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
    struct SceneActorRegistryActor *actor;
    u16 *motion;
    u32 *state;
    if (!node)
        return;
    actor = node->actor;
next:
    if (!actor)
        return;
    if (Removed(actor)) {
        void *context;
        node = node->next;
        context = actor->context;
        Callback_InvokeActor(context, actor->destroy);
        Heap_Free(context);
        actor = node->actor;
        goto next;
    }
    {
        s32 (**update)(void *) = &actor->update;
        void *context = actor->context;
        u8 missing;
        Callback_InvokeActor(context, *update);
        missing = MissingObject(actor);
        motion = &actor->flags2D4;
        if (!missing && !Paused() && !MotionFlagSet(motion, 0x4000) && !ReadStateFlags(motion, 8) &&
            actor->enabled && !ReadFlags2CC(actor, 2) && (gGameState->flags868 & 2) &&
            CurrentRegion(actor) && CanUpdate(actor, motion) && gGameState->unk876 > 0 &&
            !Stopped()) {
            actor->field178 = 0;
            ClearField2B8(actor);
            ClearDrawableFlags(actor, 1);
            actor->health = 0;
            if (ReadMotionFlags(motion, 0x2000))
                UpdateActors_SpawnContactParts(actor);
            *motion &= ~0x2000;
            SetFlags2CC(actor, 0x1000);
        }
        motion = &actor->flags2D4;
        if (!MotionFlagSet(motion, 0x4000) && TransitionEnabled(manager)) {
            s32 category = ActorCategory(actor);
            state = &actor->flags2D0;
            if (category == 10 || category == 23) {
                if (!ReadFlags2CC(actor, 0x02008000)) {
                    if (actor->drawableKind == 0)
                        ((struct SceneActorPaletteSequenceView *)actor->drawable)->paletteOffset =
                            actor->paletteBank;
                    else {
                        struct UpdateActorsTile *tile = (void *)((u8 *)actor->drawable + 0x20);
                        tile->paletteOffset = actor->paletteOffset;
                    }
                }
                if ((u8)SceneActorPalette_ApplyTransition((void *)manager, actor)) {
                    if (TransitionComplete(manager)) {
                        u32 bank;
                        manager->flags &= 0xFFFDDFFF;
                        ClearWordFlags(state, 0x200000);
                        bank = DefaultPalette(actor);
                        if (actor->drawableKind == 0)
                            AnimData_SelectPaletteBank((u8 *)actor->drawable + 0x2C, bank);
                        else
                            ((struct SceneActorRegistryTileDrawable *)actor->drawable)->palette =
                                gPaletteBankData + (bank << 5);
                    } else if ((*state & 0x200000)) {
                        SceneActorPalette_BeginTransition(
                            actor->paletteBank, actor->basePaletteBank + actor->paletteOffset, 0,
                            category, 0);
                        ClearWordFlags(state, 0x200000);
                    } else {
                        SceneActorPalette_BeginTransition(actor->basePaletteBank +
                                                              actor->paletteOffset,
                                                          actor->paletteBank, 0, category, 0);
                        *state |= 0x200000;
                    }
                }
            }
        }
        node = node->next;
        actor = node->actor;
    }
    goto next;
}
#else
INCLUDE_ASM("asm/nonmatching", SceneActorRegistry_UpdateActors);
#endif
