#include "global.h"
/* Tentative family name; visual identity is not yet established. */
struct TwinSpriteEffectSlot {
    u32 flags;
    u8 unknown4[0x2c - 4];
    u8 graphics[28];
    u8 unknown48[16];
    u16 frame;
    u16 unknown5A;
};
struct TwinSpriteEffect {
    u8 unknown0[0x3c];
    struct TwinSpriteEffectSlot sprites[2];
    u8 unknownF4[0x1aa - 0xf4];
    u8 state, count;
};
s32 AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Sprite_Init(void *, void *, u32);
void TwinSpriteEffect_InitGraphics(struct TwinSpriteEffect *effect)
{
    s32 i = 0;
    do {
        struct TwinSpriteEffectSlot *sprite = &effect->sprites[i];
        void *graphics = sprite->graphics;
        AnimData_Load(graphics, 0x354d);
        Sprite_Init(sprite, graphics, 0);
        AnimData_SelectPaletteBank(graphics, 0x2e2);
        effect->sprites[i].flags = 0x201;
        effect->sprites[i].frame = 0;
        i++;
    } while (i < 2);
    effect->state = 0;
    effect->count = 2;
}
