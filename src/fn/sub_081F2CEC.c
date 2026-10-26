#include "global.h"
#include "render.h"

#define PaletteEffect_GetPaletteIndex ActorResource_GetFixedId
#define gEffectPaletteBanks           gPaletteBankData
struct PaletteEffect {
    u8 unknown0[0x394];
    void *palette;
    u8 unknown398[0x114];
    u8 state, unknown4ad, dirty, unknown4af;
    u32 paletteIndex;
    u8 unknown4b4[4];
    u8 paletteCopy[0x20];
    u8 unknown4d8[0x3e0];
    u8 paletteDefault[0x200];
    u8 paletteAlternate[0x200];
};
s32 ActorResource_GetFixedId(struct PaletteEffect *);
static inline void CpuSetArgs(int control, u8 *dest, void *source)
{
    CpuSet(source, dest, control);
}
void PaletteEffect_RefreshPalette(struct PaletteEffect *p)
{
    s32 index, bank;
    if (p->dirty) {
        p->state = 0;
        p->dirty = 0;
        p->paletteIndex = ActorResource_GetFixedId(p);
        index = ActorResource_GetFixedId(p);
        bank = index;
        if (index == 0)
            p->palette = p->paletteDefault;
        else if (index == 501)
            p->palette = p->paletteAlternate;
        else {
            void *dest;
            void *source = gPaletteBankData + (bank << 5);
            dest = p->paletteCopy;
            CpuSetArgs(0x04000008, p->paletteCopy, source);
            p->palette = dest;
        }
    }
}
