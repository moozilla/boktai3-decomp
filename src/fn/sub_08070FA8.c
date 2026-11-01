#include "scene_actor_part_burst.h"
#include "scene_actor_palette.h"
#include "engine.h"
#include "ram.h"
#include "scene_actor_placement.h"
#include "game_state.h"
#ifdef NONMATCHING
// fuzzy: 59.7%
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
    u32 mask = 0x2000;
    if (p->flags & mask)
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
    u32 value = 0;
    if (gPlayerActionPending == 1)
        value = 1;
    return value;
}
// confidence=low: registry callback traversal, regional deactivation and palette transitions.
#define SceneActorRegistry_UpdateActors sub_08070FA8
void SceneActorRegistry_UpdateActors(struct SceneActorRegistryManager *manager)
{
    struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
    struct SceneActorRegistryActor *actor;
    if (!node)
        return;
    actor = node->actor;
    while (actor) {
        if (Removed(actor)) {
            void *context;
            node = node->next;
            context = actor->context;
            Callback_InvokeActor(context, actor->destroy);
            Heap_Free(context);
            actor = node->actor;
            continue;
        }
        Callback_InvokeActor(actor->context, actor->update);
        if (!MissingObject(actor) && !Paused() && !(actor->flags2D4 & 0x4000) &&
            !(actor->flags2D0 & 8) && actor->enabled && !(actor->flags2CC & 2) &&
            (gGameState->flags868 & 2) && CurrentRegion(actor) &&
            !SceneActorPlacement_IsUnavailable((void *)actor) && gGameState->unk876 > 0 &&
            !Stopped()) {
            actor->field178 = 0;
            actor->field2B8 = 0;
            if (actor->drawableKind == 0)
                ((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->flags &= ~1;
            else
                ((struct SceneActorRegistryTileDrawable *)actor->drawable)->flags &= ~1;
            actor->health = 0;
            if (actor->flags2D4 & 0x2000)
                SceneActorEffects_SpawnContactParts(actor);
            actor->flags2D4 &= ~0x2000;
            actor->flags2CC |= 0x1000;
        }
        if (!(actor->flags2D4 & 0x4000) && TransitionEnabled(manager)) {
            s32 category = actor->category;
            if (category == 10 || category == 23) {
                if (!(actor->flags2CC & 0x02008000)) {
                    if (actor->drawableKind == 0)
                        ((struct SceneActorPaletteSequenceView *)actor->drawable)->paletteOffset =
                            actor->paletteBank;
                    else
                        ((struct SceneActorPaletteTileView *)actor->drawable)->paletteOffset =
                            actor->paletteOffset;
                }
                if ((u8)SceneActorPalette_ApplyTransition((void *)manager, actor)) {
                    if (TransitionComplete(manager)) {
                        u32 bank;
                        manager->flags &= 0xFFFDDFFF;
                        actor->flags2D0 &= 0xFFDFFFFF;
                        bank = actor->basePaletteBank + actor->paletteOffset;
                        if (actor->drawableKind == 0)
                            AnimData_SelectPaletteBank((u8 *)actor->drawable + 0x2C, bank);
                        else
                            ((struct SceneActorRegistryTileDrawable *)actor->drawable)->palette =
                                gPaletteBankData + (bank << 5);
                    } else if (actor->flags2D0 & 0x200000) {
                        SceneActorPalette_BeginTransition(
                            actor->paletteBank, actor->basePaletteBank + actor->paletteOffset, 0,
                            category, 0);
                        actor->flags2D0 &= 0xFFDFFFFF;
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
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08070FA8);
#endif
