// MATCHING_CONCESSION: register-pinning
#include "transient_sprite_effects.h"
void Sprite_InitUnlinked(void *, void *, u32);
union TransientPackedPosition {
    struct ContactPackedVector bits;
    struct {
        u32 xy, zAndPadding;
    } words;
};
s32 TransientSpriteEffects_Initialize(struct TransientSpriteEffects *pool, void *context)
{
    s32 index;
    s32 result;
    register union TransientPackedPosition position asm("r5");
    volatile s32 savedIndex;
    gTransientSpriteEffects = pool;
    {
        u32 zPaddingMask = 0xFFFF0000;
        asm("" : : "r"(zPaddingMask));
        position.words.xy = 0;
        position.bits.z = 0;
    }
    index = 0;
    do {
        struct TransientSpriteEffect *slot;
        u32 offset = index * sizeof(*slot) + 0x18;
        struct TransientSpriteEffects *base;
        struct ContactSwarmGraphics *graphics;
        u32 loaded;
        base = pool;
        slot = (struct TransientSpriteEffect *)((u8 *)base + offset);
        slot->active = 0;
        graphics = &slot->graphics;
        {
            register struct ContactSwarmGraphics *argument asm("r0") = graphics;
            u32 resource = 0x8639;
            asm("" : "+r"(argument), "+r"(resource));
            savedIndex = index;
            loaded = AnimData_Load(argument, resource);
        }
        index = savedIndex;
        if (!loaded) {
            result = -1;
            goto done;
        }
        {
            struct TransientEffectSprite *sprite = &slot->sprite;
            register struct ContactSwarmGraphics *argument asm("r1") = graphics;
            register u32 mode asm("r2") = 0;
            savedIndex = index;
            Sprite_InitUnlinked(sprite, argument, mode);
        }
        slot->sprite.priority = 1;
        slot->sprite.position.bits = position.bits;
        index = savedIndex;
        ++index;
    } while (index <= 15);
    result = 0;
done:
    return result;
}
