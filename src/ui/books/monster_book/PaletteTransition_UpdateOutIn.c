#include "global.h"
#include "ui/books/bestiary_palette_variant.h"
void PaletteTransition_BlendBankColors(struct BestiaryPaletteVariant *, int, int);
void PaletteTransition_SetAlternateVariantParameters(struct BestiaryPaletteVariant *);
void PaletteTransition_SetVariantParameters(struct BestiaryPaletteVariant *);
s32 DisplayAnimation_TestFinished(void *);
void SceneDisplay_SwitchToAlternateDescriptor(struct BestiaryPaletteVariant *);
void PaletteTransition_BlendSelectedBanks(struct BestiaryPaletteVariant *, int, int);

void PaletteTransition_UpdateOutIn(struct BestiaryPaletteVariant *p)
{
    s32 phase = p->frame & 0x3f;
    if (phase > 0x1f)
        phase = 0x3f - phase;
    switch (p->state) {
        case 0:
            p->flag = 1;
            PaletteTransition_BlendSelectedBanks(p, 0x40 - p->frame, 6);
            if (p->frame > 0x3f) {
                p->state++;
                return;
            }
            break;
        case 1:
            if (DisplayAnimation_TestFinished(p->anim)) {
                p->state++;
                p->frame = 0;
                p->counter++;
                return;
            }
            break;
        case 2:
            if (DisplayAnimation_TestFinished(p->anim)) {
                PaletteTransition_SetVariantParameters(p);
                p->state++;
                p->frame = 0;
                p->flag = 1;
                return;
            }
            break;
        case 3:
            PaletteTransition_BlendBankColors(p, phase, 5);
            if (p->frame > 0x78) {
                PaletteTransition_SetAlternateVariantParameters(p);
                p->state++;
            }
            break;
        case 4:
            PaletteTransition_BlendBankColors(p, phase, 5);
            if (DisplayAnimation_TestFinished(p->anim)) {
                p->state++;
                p->frame = 0;
                p->flag = 1;
            }
            break;
        case 5:
            if (p->frame > 0x3f) {
                SceneDisplay_SwitchToAlternateDescriptor(p);
                PaletteTransition_BlendSelectedBanks(p, 0x40, 6);
                return;
            }
            p->flag = 1;
            PaletteTransition_BlendSelectedBanks(p, p->frame, 6);
            break;
    }
    p->frame++;
}
