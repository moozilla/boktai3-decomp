#include "scene_actor_part_burst.h"
#include "scene_actor_palette.h"
#include "engine.h"
#include "ram.h"
#include "scene_actor_placement.h"
#include "game_state.h"
#ifdef NONMATCHING
// fuzzy: 66.7%
// Remaining: register allocation. The ROM keeps the PRE'd &flags2D4 in sl (the
// draft gives sl to &velocity and spills &flags2D4), the frame is 0x44 (draft
// 0x40), and in the palette branches the ROM derives &paletteOffset from
// &paletteBank's constant (adds r7, #8). Structure that now matches: union table
// view (loop.c keeps the load in the pass), count-up loop (dbra reversal places
// the counter), effectOrigin re-read per field, a goto loop (nothing hoisted out
// of the outer loop) and inline flag helpers (constant-offset forms that gcse
// shares).
void Heap_Free(void *);

static inline u8 Removed(struct SceneActorRegistryActor *p)
{
    u32 mask = 0x1000;
    if (mask & p->flags2D6)
        return 1;
    return 0;
}
static inline u8 MissingObject(struct SceneActorRegistryActor *p)
{
    if (!p->object)
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
static inline void UpdateActors_SpawnContactParts(struct SceneActorRegistryActor *actor)
{
    union SceneActorRegistryPackedPosition velocity, bounds, position;
    s32 i;
    for (i = 0; i < 3; i++) {
        union RandomTableView table = {gRandomTable};
        const struct ContactPosition *origin;
        velocity.packed.x = (SceneActorEffects_NextSample(table.samples) & 15) - 7;
        velocity.packed.y = (SceneActorEffects_NextSample(table.samples) & 31) - 16;
        velocity.packed.z = (SceneActorEffects_NextSample(table.samples) & 15) - 7;
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
static inline u16 ReadFlags2D4(struct SceneActorRegistryActor *p)
{
    return p->flags2D4;
}
static inline void ClearField2B8(struct SceneActorRegistryActor *p)
{
    p->field2B8 = 0;
}
static inline void SetFlags2CC(struct SceneActorRegistryActor *p, u32 bits)
{
    p->flags2CC |= bits;
}
static inline void ClearFlags2D4(struct SceneActorRegistryActor *p, u32 bits)
{
    p->flags2D4 &= ~bits;
}
static inline u32 DefaultPalette(struct SceneActorRegistryActor *actor)
{
    u16 *base = &actor->basePaletteBank;
    u16 *offset = &actor->paletteOffset;
    return *base + *offset;
}
// confidence=low: registry callback traversal, regional deactivation and palette transitions.
void SceneActorRegistry_UpdateActors(struct SceneActorRegistryManager *manager)
{
    struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
    struct SceneActorRegistryActor *actor;
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
        Callback_InvokeActor(actor->context, actor->update);
        if (!MissingObject(actor) && !Paused() &&
            !SceneActorPlacement_ReadFlags16((void *)actor, 0x4000) &&
            !SceneActorPlacement_ReadFlags32((void *)actor, 8) && actor->enabled &&
            !ReadFlags2CC(actor, 2) && (gGameState->flags868 & 2) && CurrentRegion(actor) &&
            !SceneActorPlacement_IsUnavailable((void *)actor) && gGameState->unk876 > 0 &&
            !Stopped()) {
            actor->field178 = 0;
            ClearField2B8(actor);
            ClearDrawableFlags(actor, 1);
            actor->health = 0;
            if (ReadFlags2D4(actor) & 0x2000)
                UpdateActors_SpawnContactParts(actor);
            ClearFlags2D4(actor, 0x2000);
            SetFlags2CC(actor, 0x1000);
        }
        if (!SceneActorPlacement_ReadFlags16((void *)actor, 0x4000) && TransitionEnabled(manager)) {
            s32 category = actor->category;
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
                        ClearWordFlags(&actor->flags2D0, 0x200000);
                        bank = DefaultPalette(actor);
                        if (actor->drawableKind == 0)
                            AnimData_SelectPaletteBank((u8 *)actor->drawable + 0x2C, bank);
                        else
                            ((struct SceneActorRegistryTileDrawable *)actor->drawable)->palette =
                                gPaletteBankData + (bank << 5);
                    } else if (SceneActorPlacement_ReadFlags32((void *)actor, 0x200000)) {
                        SceneActorPalette_BeginTransition(
                            actor->paletteBank, actor->basePaletteBank + actor->paletteOffset, 0,
                            category, 0);
                        ClearWordFlags(&actor->flags2D0, 0x200000);
                    } else {
                        SceneActorPalette_BeginTransition(actor->basePaletteBank +
                                                              actor->paletteOffset,
                                                          actor->paletteBank, 0, category, 0);
                        actor->flags2D0 |= 0x200000;
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
