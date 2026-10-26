#include "scene_actor_palette.h"
#ifdef NONMATCHING
// fuzzy: 57.7%
extern u16 gUnk_02000528;
extern u32 gUnk_03005308;
void Heap_Free(void *);
void EffectGroup_SpawnPart(void *, s32, void *, void *, void *, s32, s32);
union RegistryParticleVector {
    struct ContactPosition position;
    struct {
        s32 x : 16, y : 16, z : 16, pad : 16;
    } packed;
};
static inline u32 RandomValue(void)
{
    u32 index = (gUnk_03005308 + 1) & 1023;
    gUnk_03005308 = index;
    return ((const u16 *)RANDOM_TABLE_ADDR)[index];
}
static inline u8 Removed(struct SceneActorRegistryActor *p)
{
    if (p->flags2D6 & 0x1000)
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
    if (gUnk_030053F4 & 0x4800)
        return 1;
    return 0;
}
static inline u8 CurrentRegion(struct SceneActorRegistryActor *p)
{
    if (*(u32 *)((u8 *)gUnk_02000710 + 0x5A0) == p->regionId)
        return 1;
    return 0;
}
static inline u8 Ineligible(struct SceneActorRegistryActor *p)
{
    if ((p->flags2D4 & 0x40) || (p->flags2D0 & 0x20000) || p->health <= 0)
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
    if (p->flags & 0x20000)
        return 1;
    return 0;
}
static inline u8 Stopped(void)
{
    u32 value = 0;
    if (gUnk_02000528 == 1)
        value = 1;
    return value;
}
// confidence=low: registry callback traversal, regional deactivation and palette transitions.
#define SceneActorRegistry_UpdateActors sub_08070FA8
void SceneActorRegistry_UpdateActors(struct SceneActorRegistryManager *manager)
{
    struct SceneActorRegistryNode *node = gUnk_020004AC;
    struct SceneActorRegistryActor *actor;
    union RegistryParticleVector velocity, bounds, position;
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
            if (!node)
                return;
            actor = node->actor;
            continue;
        }
        Callback_InvokeActor(actor->context, actor->update);
        if (!MissingObject(actor) && !Paused() && !(actor->flags2D4 & 0x4000) &&
            !(actor->flags2D0 & 8) && actor->enabled && !(actor->flags2CC & 2) &&
            (*(u32 *)((u8 *)gUnk_02000710 + 0x868) & 2) && CurrentRegion(actor) &&
            !Ineligible(actor) && *(s16 *)((u8 *)gUnk_02000710 + 0x876) > 0 && !Stopped()) {
            actor->field178 = 0;
            actor->field2B8 = 0;
            if (actor->drawableKind == 0)
                ((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->flags &= ~1;
            else
                ((struct SceneActorRegistryTileDrawable *)actor->drawable)->flags &= ~1;
            actor->health = 0;
            if (actor->flags2D4 & 0x2000) {
                s32 i = 2;
                do {
                    velocity.packed.x = (RandomValue() & 15) - 7;
                    velocity.packed.y = (RandomValue() & 31) - 16;
                    velocity.packed.z = (RandomValue() & 15) - 7;
                    bounds.packed.x = 5;
                    bounds.packed.y = 10;
                    bounds.packed.z = 5;
                    position.packed.x = actor->effectOrigin->x;
                    position.packed.y = actor->effectOrigin->y + 128;
                    position.packed.z = actor->effectOrigin->z;
                    EffectGroup_SpawnPart(actor->effectConfig, 3, &position, &velocity, &bounds, 40,
                                          40);
                    i--;
                } while (i >= 0);
                actor->fieldAA = 0;
                actor->field2B8 = 0;
            }
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
