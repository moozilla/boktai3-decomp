#include "scene_actor_part_burst.h"
#include "scene_actor_palette.h"
#include "engine.h"
#include "ram.h"
#include "scene_actor_placement.h"
#include "game_state.h"
void Heap_Free(void *);

static inline u8 RemovalRequested(struct SceneActorRegistryActor *p)
{
    u32 mask = 0x1000;
    if (mask & p->flags2D6)
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

static inline u8 CurrentRegion(struct SceneActorRegistryActor *p)
{
    u16 region = p->regionId;
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
static inline void UpdateActors_SpawnContactParts(struct SceneActorRegistryActor *actor)
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

static inline u32 ReadActorStateFlags(struct SceneActorRegistryActor *p, u32 mask)
{
    return p->flags2D0 & mask;
}
static inline u8 ActorUnavailable(struct SceneActorRegistryActor *p, u16 *flags)
{
    u32 mask = 0x40;
    if (p) {
        if ((mask & *flags) || SceneActorPlacement_ReadFlags32((void *)p, 0x20000) ||
            p->health <= 0)
            return 1;
    }
    return 0;
}

static inline u32 *StateAddress(struct SceneActorRegistryActor *p)
{
    return &p->flags2D0;
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
    struct SceneActorRegistryActor *actor;
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
        context = actor->context;
        Callback_InvokeActor(context, actor->destroy);
        Heap_Free(context);
        actor = node->actor;
        goto next;
    }
    {
        s32 (**update)(void *) = &actor->update;
        void *context = actor->context;
        u8 missing, paused;
        Callback_InvokeActor(context, *update);
        missing = MissingObject(actor);
        motion = &actor->flags2D4;
        if (missing)
            goto palette;
        paused = UpdatesPaused();
        motion = &actor->flags2D4;
        if (paused)
            goto palette;
        flagField = (void *)motion;
        if (!MotionFlagSet(&flagField->halfword, 0x4000) && !ReadActorStateFlags(actor, 8) &&
            actor->enabled && !ReadFlags2CC(actor, 2) && (RegistryCurrentGame()->flags868 & 2) &&
            CurrentRegion(actor) && !ActorUnavailable(actor, motion) &&
            RegistryCurrentGame()->unk876 > 0 && !PlayerActionPending()) {
            actor->field178 = 0;
            ClearField2B8(actor);
            ClearDrawableFlags(actor, 1);
            {
                s16 zero = 0;
                actor->health = zero;
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
                    if (actor->drawableKind == 0) {
                        draw = actor->drawable;
                        ((struct SceneActorPaletteSequenceView *)draw)->paletteOffset =
                            actor->paletteBank;
                    } else {
                        struct UpdateActorsTile *tile;
                        draw = actor->drawable;
                        tile = (void *)((u8 *)draw + 0x20);
                        tile->paletteOffset = actor->paletteOffset;
                    }
                }
                if ((u8)SceneActorPalette_ApplyTransition((void *)manager, actor)) {
                    if (TransitionComplete(manager)) {
                        u32 bank;
                        manager->flags &= 0xFFFDDFFF;
                        flagField = (void *)&actor->flags2D0;
                        ClearWordFlags(&flagField->word, 0x200000);
                        bank = DefaultPalette(actor);
                        if (actor->drawableKind == 0)
                            AnimData_SelectPaletteBank((u8 *)actor->drawable + 0x2C, bank);
                        else
                            ((struct SceneActorRegistryTileDrawable *)actor->drawable)->palette =
                                gPaletteBankData + (bank << 5);
                    } else {
                        u32 tested, bit;
                        bit = 0x200000;
                        tested = SceneActorPlacement_ReadFlags32((void *)actor, bit);
                        flagField = (void *)StateAddress(actor);
                        if (tested) {
                            u16 bank = actor->paletteBank;
                            SceneActorPalette_BeginTransition(bank, DefaultPalette(actor), 0,
                                                              category, 0);
                            ClearWordFlags(&flagField->word, 0x200000);
                        } else {
                            SceneActorPalette_BeginTransition(DefaultPalette(actor),
                                                              actor->paletteBank, 0, category, 0);
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
