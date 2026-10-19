#include "global.h"
#include "level_advance_display.h"

#ifdef NONMATCHING
// fuzzy: 90.9%
// foundry draft (worker best 90.9%); what differs is mostly register allocation and instruction order
#define ParticleSprite_Initialize  sub_08217DE0
#define ParticleSprite_SetOffset   Effect_SetOrigin
#define ParticleSprite_SetResource SpriteDrawable_SelectTileFrame
#define ResourceSlots_Load         Palette_CacheReservedObjectBank
#define SpriteQueue_Insert         Effect_LinkToList
#define gSpritePaletteData         gUnk_030042E4
u32 Palette_CacheReservedObjectBank(u32, void *);
s32 Effect_LinkToList(struct LevelAdvanceParticle *, u32);
extern u8 *gUnk_030042E4;
static inline u8 selectQueue(u32 flags)
{
    return (flags & 0x80) != 0;
}
void sub_08217DE0(struct LevelAdvanceParticle *sprite, void *resource, u32 flags)
{
    u32 zero = sprite->queued;
    if (!zero) {
        sprite->flags = flags;
        sprite->frameState = zero;
        sprite->mode = 2;
        sprite->status = zero;
        sprite->rotation = zero;
        sprite->scaleX = 0x40;
        sprite->scaleY = 0x40;
        Effect_SetOrigin(sprite, 0, 0);
        SpriteDrawable_SelectTileFrame(sprite, resource, 0);
        sprite->paletteSlot = Palette_CacheReservedObjectBank(
            *(u16 *)((u8 *)resource + 6), gUnk_030042E4 + (*(u16 *)((u8 *)resource + 6) << 5));
        {
            u32 queue = selectQueue(flags);
            sprite->previous = (void *)zero;
            sprite->next = (void *)zero;
            Effect_LinkToList(sprite, queue);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08217DE0);
#endif
