#include "sprite_effect_controller.h"
#include "engine.h"
#include "ram.h"
#include "player_cue_sequence.h"
struct SequenceDrawable {
    u32 flags;
    u8 active, unknown5[2], priority;
    u8 unknown8[4];
    void *graphics;
    u16 frame;
    u8 unknown12[0xa];
    struct ContactPosition position;
    u8 unknown24[8];
    u8 resource[0x1c];
};
struct TileDrawable {
    u32 flags;
    u8 active;
    u8 unknown5[3];
    u16 tile;
    u8 width, height;
    u8 biasX, biasY, unknownE, priority;
    u8 unknown10[8];
    struct ContactPosition position;
    u8 unknown20[8];
    void *graphics;
};
struct SpriteEffectControllerPool *SpriteEffectController_EnsurePool(void);
void *SpriteEffectController_LoadSpriteResources(struct SpriteEffectController *, s32, u32);
void ParticleSprite_Initialize(void *, void *, s32);
void Effect_SetOrigin(void *, s32, s32);
void Effect_SetPalette(void *, s32);
extern u32 gUnk_02000560;
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
    if (gUnk_02000560 & 4)
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
        struct SequenceDrawable *sprite;
        slot->drawable = Heap_Alloc(0x48);
        Memory_Clear(slot->drawable, 0x48);
        sprite = slot->drawable;
        sprite->position = *position;
        resource = SpriteEffectController_LoadSpriteResources(slot, graphicsId, context);
        Sprite_Init(sprite, (u8 *)slot->drawable + 0x2c, 0);
        Anim_SetSequence(&slot->sequence, resource, (u16)sequence, (u8)variant, (u8)animationFlags);
        AnimData_SelectPaletteBank(sprite->graphics, palette);
        PlayerCue_StepSequence((struct ContactSwarmSprite *)sprite, &slot->sequence);
        sprite->flags |= 1 | drawableFlags;
        sprite->priority = priority;
    } else if (slot->flags & 0x10) {
        struct TileDrawable *sprite;
        slot->drawable = Heap_Alloc(0x2c);
        Memory_Clear(slot->drawable, 0x2c);
        sprite = slot->drawable;
        sprite->position = *position;
        {
            struct TileDrawable *graphicsOwner = slot->drawable;
            graphicsOwner->graphics = AnimData_FindEntry((u16)graphicsId);
        }
        resource = ScriptProc_Start(0x922e, context);
        ParticleSprite_Initialize(sprite, ((struct TileDrawable *)slot->drawable)->graphics, 0);
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
