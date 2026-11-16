#include "scene_actor_part_burst.h"
#include "scene_actor_palette.h"
#include "engine.h"
#include "ram.h"
#include "scene_actor_placement.h"
#include "game_state.h"
void Heap_Free(void *);

static inline u8 RemovalRequested(struct ChaseActor *p)
{
    u32 mask = 0x1000;
    if (mask & p->base.reactionFlags)
        return 1;
    return 0;
}
static inline u8 UpdatesPaused(void)
{
    if (gEngineFlags & 0x4800)
        return 1;
    return 0;
}
static inline struct GameState *RegistryCurrentGame(void)
{
    struct GameState *game;
    do {
        game = gGameState;
    } while (0);
    return game;
}

static inline u8 CurrentRegion(struct ChaseActor *p)
{
    u16 region = p->base.regionId;
    if (RegistryCurrentGame()->region == region)
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
static inline u8 PlayerActionPending(void)
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
static inline void UpdateActors_SpawnContactParts(struct ChaseActor *actor)
{
    union SceneActorRegistryPackedPosition velocity, bounds, position;
    u32 horizontalMask = 15;
    s32 i;
    for (i = 0; i < 3; i++) {
        union RandomTableView table = {gRandomTable};
        s16 velocityComponent;
        velocityComponent = (NextParticleSample(table.samples) & horizontalMask) - 7;
        velocity.packed.x = velocityComponent;
        velocityComponent = ((s8)NextParticleSample(table.samples) & 31) - 16;
        velocity.packed.y = velocityComponent;
        velocityComponent = ((s8)NextParticleSample(table.samples) & 15) - 7;
        velocity.packed.z = velocityComponent;
        bounds.packed.x = 5;
        bounds.packed.y = 10;
        bounds.packed.z = 5;
        position.packed.x = actor->group.effectOrigin->x;
        position.packed.y = actor->group.effectOrigin->y + 128;
        position.packed.z = actor->group.effectOrigin->z;
        EffectGroup_SpawnPart((void *)actor->group.effectConfig, 3, &position.position,
                              &velocity.position, &bounds.position, 40, 40);
    }
    actor->base.attachmentTimer = 0;
    actor->base.hitLockTimer = 0;
}

static inline u32 ReadFlags2CC(struct ChaseActor *p, u32 mask)
{
    return p->base.behaviorFlags & mask;
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
static inline void ClearDrawableFlags(struct ChaseActor *actor, u32 bits)
{
    if (actor->base.drawableKind == 0)
        ClearWordFlags(&((struct SceneActorRegistrySequenceDrawable *)actor->base.drawable)->flags,
                       bits);
    else {
        struct UpdateActorsTile *tile = (void *)((u8 *)actor->base.drawable + 0x20);
        ClearTileFlags(tile, bits);
    }
}
static inline void ClearField2B8(struct ChaseActor *p)
{
    p->base.hitLockTimer = 0;
}
static inline void SetFlags2CC(struct ChaseActor *p, u32 bits)
{
    p->base.behaviorFlags |= bits;
}
static inline u8 ActorCategory(struct ChaseActor *p)
{
    return p->base.category;
}

static inline u32 DefaultPalette(struct ChaseActor *actor)
{
    u16 *base = &actor->base.basePaletteBank;
    u16 *offset = &actor->base.paletteOffset;
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
static inline u8 MissingObject(struct ChaseActor *p)
{
    if (!p->base.object)
        return 1;
    return 0;
}

static inline u32 ReadActorStateFlags(struct ChaseActor *p, u32 mask)
{
    return p->base.statusFlags & mask;
}
static inline u8 ActorUnavailable(struct ChaseActor *p, u16 *flags)
{
    u32 mask = 0x40;
    if (p) {
        if ((mask & *flags) || SceneActorPlacement_ReadFlags32((void *)p, 0x20000) ||
            p->base.health <= 0)
            return 1;
    }
    return 0;
}

static inline u32 *StateAddress(struct ChaseActor *p)
{
    return &p->base.statusFlags;
}

static inline u32 ClearMaskedValue(u32 flags, u32 bits)
{
    u32 mask = ~bits;
    mask &= flags;
    return mask;
}

static inline u8 ManagerFlagSet(struct SceneActorRegistryManager *m, u32 mask)
{
    u32 flags = m->flags;
    if (flags & mask)
        return 1;
    return 0;
}

union RegistryFlagField {
    u32 word;
    u16 halfword;
};

// confidence=high: walks the registry, runs actor callbacks and updates palette transitions.
void SceneActorRegistry_UpdateActors(struct SceneActorRegistryManager *manager)
{
    struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
    struct ChaseActor *actor;
    void *draw;
    u32 category;
    u32 partMask = 0x2000;
    u16 *motion;
    union RegistryFlagField *flagField;
    if (!node)
        return;
    actor = node->actor;
next:
    if (!actor)
        return;
    if (RemovalRequested(actor)) {
        void *context;
        node = node->next;
        context = actor->base.context;
        Callback_InvokeActor(context, actor->base.destroy);
        Heap_Free(context);
        actor = node->actor;
        goto next;
    }
    {
        s32 (**update)(void *) = &actor->base.update;
        void *context = actor->base.context;
        u8 missing, paused;
        Callback_InvokeActor(context, *update);
        missing = MissingObject(actor);
        motion = &actor->base.motionFlags;
        if (missing)
            goto palette;
        paused = UpdatesPaused();
        motion = &actor->base.motionFlags;
        if (paused)
            goto palette;
        flagField = (void *)motion;
        if (!MotionFlagSet(&flagField->halfword, 0x4000) && !ReadActorStateFlags(actor, 8) &&
            actor->base.enabled && !ReadFlags2CC(actor, 2) &&
            (RegistryCurrentGame()->flags868 & 2) && CurrentRegion(actor) &&
            !ActorUnavailable(actor, motion) && RegistryCurrentGame()->unk876 > 0 &&
            !PlayerActionPending()) {
            actor->base.colliderA.responseValue = 0;
            ClearField2B8(actor);
            ClearDrawableFlags(actor, 1);
            {
                s16 zero = 0;
                actor->base.health = zero;
            }
            if (ReadMotionFlags(motion, partMask))
                UpdateActors_SpawnContactParts(actor);
            *motion = ClearMaskedValue(*motion, 0x2000);
            SetFlags2CC(actor, 0x1000);
        }
    palette:

        flagField = (void *)motion;
        if (!MotionFlagSet(&flagField->halfword, 0x4000) && ManagerFlagSet(manager, partMask)) {
            category = ActorCategory(actor);
            if (category == 10 || category == 23) {
                if (!ReadFlags2CC(actor, 0x02008000)) {
                    if (actor->base.drawableKind == 0) {
                        draw = actor->base.drawable;
                        ((struct SceneActorPaletteSequenceView *)draw)->paletteOffset =
                            actor->base.paletteBank;
                    } else {
                        struct UpdateActorsTile *tile;
                        draw = actor->base.drawable;
                        tile = (void *)((u8 *)draw + 0x20);
                        tile->paletteOffset = actor->base.paletteOffset;
                    }
                }
                if ((u8)SceneActorPalette_ApplyTransition((void *)manager, actor)) {
                    if (TransitionComplete(manager)) {
                        u32 bank;
                        manager->flags &= 0xFFFDDFFF;
                        flagField = (void *)&actor->base.statusFlags;
                        ClearWordFlags(&flagField->word, 0x200000);
                        bank = DefaultPalette(actor);
                        if (actor->base.drawableKind == 0)
                            AnimData_SelectPaletteBank((u8 *)actor->base.drawable + 0x2C, bank);
                        else
                            ((struct SceneActorRegistryTileDrawable *)actor->base.drawable)
                                ->palette = gPaletteBankData + (bank << 5);
                    } else {
                        u32 tested, bit;
                        bit = 0x200000;
                        tested = SceneActorPlacement_ReadFlags32((void *)actor, bit);
                        flagField = (void *)StateAddress(actor);
                        if (tested) {
                            u16 bank = actor->base.paletteBank;
                            SceneActorPalette_BeginTransition(bank, DefaultPalette(actor), 0,
                                                              category, 0);
                            ClearWordFlags(&flagField->word, 0x200000);
                        } else {
                            SceneActorPalette_BeginTransition(
                                DefaultPalette(actor), actor->base.paletteBank, 0, category, 0);
                            flagField->word |= bit;
                        }
                    }
                }
            }
        }
        node = node->next;
        actor = node->actor;
    }
    goto next;
}
