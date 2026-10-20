#include "global.h"
struct PaletteTransition {
    u8 pad0[0x20];
    u32 frame;
    u8 state;
    u8 pad25[3];
    u8 anim[1];
};
void PaletteTransition_BlendBankColors(struct PaletteTransition *, int, int);
void PaletteTransition_SetAlternateVariantParameters(struct PaletteTransition *);
s32 DisplayAnimation_TestFinished(void *);
void SceneDisplay_SwitchToAlternateDescriptor(struct PaletteTransition *);
void PaletteTransition_BlendSelectedBanks(struct PaletteTransition *, int, int);
void PaletteTransition_UpdateFade(struct PaletteTransition *p)
{
    s32 phase = p->frame & 0x3f;
    if (phase > 0x1f)
        phase = 0x3f - phase;
    switch (p->state) {
        case 0:
            PaletteTransition_BlendBankColors(p, phase, 5);
            if (p->frame > 0x78) {
                PaletteTransition_SetAlternateVariantParameters(p);
                p->state++;
            }
            break;
        case 1:
            PaletteTransition_BlendBankColors(p, phase, 5);
            if (DisplayAnimation_TestFinished(p->anim)) {
                p->state++;
                p->frame = 0;
                *((u8 *)p + 0xa02) = 1;
            }
            break;
        case 2:
            if (p->frame > 0x3f) {
                SceneDisplay_SwitchToAlternateDescriptor(p);
                PaletteTransition_BlendSelectedBanks(p, 0x40, 6);
                return;
            }
            *((u8 *)p + 0xa02) = 1;
            PaletteTransition_BlendSelectedBanks(p, p->frame, 6);
            break;
    }
    p->frame++;
}
