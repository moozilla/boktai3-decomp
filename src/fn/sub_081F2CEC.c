
#ifdef NONMATCHING
// fuzzy: 98.6%
// foundry draft (worker best 98.6%); what differs is mostly register allocation and instruction order
#define PaletteEffect_RefreshPalette  sub_081F2CEC
#define PaletteEffect_GetPaletteIndex ActorResource_GetFixedId
#define gEffectPaletteBanks           gUnk_030042E4
#include "global.h"
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
extern u8 *gUnk_030042E4;
s32 ActorResource_GetFixedId(struct PaletteEffect *);
void sub_081F2CEC(struct PaletteEffect *p)
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
            void *source = gUnk_030042E4 + (bank << 5);
            dest = p->paletteCopy;
            CpuSet(source, p->paletteCopy, 0x04000008);
            p->palette = dest;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081F2CEC);
#endif
