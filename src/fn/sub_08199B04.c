#include "global.h"
#define SceneEffect_UpdatePulsePalette sub_08199B04
#define Palette_BlendColors sub_08221110
#define gSceneEffectPaletteBanks gUnk_030042E4
#define gSceneEffectPalettePhase gUnk_03003A50

struct PulsePaletteOwner { u8 pad[0x24]; u16 delay; };
struct SceneEffectPulse {
    u8 pad00[0xe];
    u16 paletteId;
    u8 pad10[4];
    void *palette;
    u8 pad18[0x94];
    struct PulsePaletteOwner *owner;
    u16 colors[16];
    u16 timer;
};
extern u8 *gUnk_030042E4;
extern u32 gUnk_03003A50;
void sub_08221110(void *, void *, void *, u32, u32);

void sub_08199B04(struct SceneEffectPulse *effect)
{
    struct PulsePaletteOwner *owner = effect->owner;
    if (effect->timer < owner->delay) {
        effect->timer++;
    } else {
        void *from = gUnk_030042E4 + 0x11e0;
        void *to = gUnk_030042E4 + 0x1200;
        u32 phase = gUnk_03003A50;
        u32 amount = phase & 0x3f;
        effect->paletteId = 0xf08f;
        effect->palette = effect->colors;
        if (phase & 0x40)
            sub_08221110(effect->colors, from, to, amount, 6);
        else
            sub_08221110(effect->colors, from, to, 0x40 - amount, 6);
    }
}
