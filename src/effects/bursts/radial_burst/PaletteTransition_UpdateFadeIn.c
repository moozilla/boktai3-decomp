#define PaletteTransition_BlendFromGray RadialBurstEffect_BlendSpritePalette
#include "global.h"
struct PaletteTransition {
    u32 flags;
    u8 unknown4[0x18];
    u8 position[0x2c];
    const u16 *source;
    u16 colors[16];
    u8 unknown6C;
    u8 alpha;
    u8 unknown6E[4];
    u16 ticks;
    void *owner;
};
void RadialBurstEffect_BlendSpritePalette(struct PaletteTransition *);
void CallbackGroup_EmitAtPosition(void *, void *);
void PaletteTransition_UpdateFadeIn(struct PaletteTransition *transition)
{
    if (transition->alpha <= 5)
        transition->flags |= 0x400;
    else
        transition->flags &= ~0x400;
    if (transition->alpha <= 15) {
        transition->alpha++;
        RadialBurstEffect_BlendSpritePalette(transition);
    }
    if (++transition->ticks <= 19)
        CallbackGroup_EmitAtPosition(transition->owner, transition->position);
}
