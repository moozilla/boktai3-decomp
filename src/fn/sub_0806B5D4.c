#include "sprite_effect_controller.h"
#include "engine.h"
#include "ram.h"
#include "player_cue_sequence.h"
#include "player.h"
struct SpriteEffectControllerPool *SpriteEffectController_EnsurePool(void);
void *SpriteEffectController_LoadSpriteResources(struct SpriteEffectController *, s32, u32);
void ParticleSprite_Initialize(void *, const void *, s32);
void Effect_SetOrigin(void *, s32, s32);
void Effect_SetPalette(void *, s32);
struct SpriteEffectController *
SpriteEffectController_Create(const struct ContactPosition *position,
                              const struct ContactPosition *velocity, s32 graphicsId, u32 context,
                              u32 sequence, u32 drawableFlags, u32 flags, u32 palette, u32 priority,
                              u32 variant, u32 animationFlags, u32 lifetime, s32 contactA,
                              s32 contactB, s32 contactC, s32 contactD, s32 biasX, s32 biasY)
{
    struct SpriteEffectControllerPool *p;
    struct SpriteEffectController *slot;
    void *resource;
    p = gSpriteEffectControllerPool;
    if (!p) {
        p = SpriteEffectController_EnsurePool();
        if (!p)
            return 0;
    }
    {
        u32 mask = 0x400;
        if ((gEngineFlags | gScenePauseFlags) & mask)
            return 0;
    }
    if (gPlayerSlotStatus[0] & 4)
        return 0;
    slot = SpriteEffectController_AllocateSlot(p);
    if (!slot)
        return 0;
    slot->flags = flags;
    slot->enter = 1;
    slot->state = 1;
    slot->velocity = *velocity;
    if (velocity->x * velocity->x + velocity->y * velocity->y + velocity->z * velocity->z)
        slot->flags |= 0x80;
    slot->lifetime = lifetime;
    if (slot->flags & 8) {
        struct SpriteEffectSequenceDrawable *sprite;
        slot->drawable = Heap_Alloc(sizeof(*sprite));
        Memory_Clear(slot->drawable, sizeof(*sprite));
        sprite = slot->drawable;
        sprite->position = *position;
        resource = SpriteEffectController_LoadSpriteResources(slot, graphicsId, context);
        Sprite_Init(sprite, &((struct SpriteEffectSequenceDrawable *)slot->drawable)->resource, 0);
        Anim_SetSequence(&slot->sequence, resource, (u16)sequence, (u8)variant, (u8)animationFlags);
        AnimData_SelectPaletteBank(sprite->graphics, palette);
        PlayerCue_StepSequence((struct ContactSwarmSprite *)sprite, &slot->sequence);
        sprite->flags |= 1 | drawableFlags;
        sprite->priority = priority;
    } else if (slot->flags & 0x10) {
        struct SpriteEffectTileDrawable *sprite;
        slot->drawable = Heap_Alloc(sizeof(*sprite));
        Memory_Clear(slot->drawable, sizeof(*sprite));
        sprite = slot->drawable;
        sprite->position = *position;
        {
            struct SpriteEffectTileDrawable *graphicsOwner = slot->drawable;
            graphicsOwner->graphics = AnimData_FindEntry((u16)graphicsId);
        }
        resource = ScriptProc_Start(0x922e, context);
        ParticleSprite_Initialize(sprite,
                                  ((struct SpriteEffectTileDrawable *)slot->drawable)->graphics, 0);
        Effect_SetOrigin(sprite, -biasX, -biasY);
        Anim_SetSequence(&slot->sequence, resource, (u16)sequence, (u8)variant, (u8)animationFlags);
        Effect_SetPalette(sprite, palette);
        sprite->flags |= 1 | drawableFlags;
        sprite->priority = priority;
        if (slot->flags & 0x20)
            slot->state = 2;
        else if (slot->flags & 0x200)
            slot->state = 3;
    }
    if (contactA > 0) {
        SpriteEffectController_InitCollider(slot, contactA, contactB, contactC, contactD);
        slot->flags |= 4;
    }
    return slot;
}
