#include "global.h"
#define EffectVariant_SetProperties sub_081A32EC
#define gEffectVariantValues        gUnk_08611EB8
#define gEffectVariantFlagModes     gUnk_08611EF8
extern const u32 gUnk_08611EB8[];
extern const s32 gUnk_08611EF8[];
struct EffectVariant {
    u32 flags;
    u8 unknown4[2];
    u8 variant;
    u8 unknown7[9];
    u16 value;
};
void sub_081A32EC(struct EffectVariant *effect, s32 variant)
{
    s32 index = (variant & 0xFF) >> 4;
    effect->variant = variant;
    effect->value = gEffectVariantValues[index];
    effect->flags &= ~12;
    switch (gEffectVariantFlagModes[index]) {
        case 1:
            effect->flags |= 4;
            break;
        case 2:
            effect->flags |= 8;
            break;
        case 3:
            effect->flags |= 12;
            break;
    }
}
