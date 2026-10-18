#define PaletteTransition_UpdateFadeIn  sub_0823659C
#define PaletteTransition_BlendFromGray RadialBurstEffect_BlendSpritePalette
#define CallbackGroup_EmitAtPosition    sub_08235EE0
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
void sub_08235EE0(void *, void *);
void sub_0823659C(struct PaletteTransition *transition)
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
        sub_08235EE0(transition->owner, transition->position);
}
