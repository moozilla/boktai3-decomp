#include "global.h"
#include "render.h"
#include "ram.h"
#define gSceneEffectPaletteBanks gPaletteBankData

struct PulsePaletteOwner {
    u8 pad[0x24];
    u16 delay;
};
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
void Palette_InterpolateBanks(void *, void *, void *, u32, u32);

void SceneEffect_UpdatePulsePalette(struct SceneEffectPulse *effect)
{
    struct PulsePaletteOwner *owner = effect->owner;
    if (effect->timer < owner->delay) {
        effect->timer++;
    } else {
        void *from = gPaletteBankData + 0x11e0;
        void *to = gPaletteBankData + 0x1200;
        u32 phase = gSceneEffectPalettePhase;
        u32 amount = phase & 0x3f;
        effect->paletteId = 0xf08f;
        effect->palette = effect->colors;
        if (phase & 0x40)
            Palette_InterpolateBanks(effect->colors, from, to, amount, 6);
        else
            Palette_InterpolateBanks(effect->colors, from, to, 0x40 - amount, 6);
    }
}
