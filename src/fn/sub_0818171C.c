#include "global.h"

#ifdef NONMATCHING
// fuzzy: 95.2%
// foundry draft (worker best 95.2%); what differs is mostly register allocation and instruction order
#define ActorPalette_LoadScriptBanks sub_0818171C
struct ActorPaletteSource {
    u8 unknown0[0x74];
    u16 banks[3][16];
};
struct ActorPaletteOwner {
    u8 unknown0[0x650];
    struct {
        u32 callbacks[2];
        u16 banks[16][16];
    } palettes;
    u8 unknown858[0x282];
    u8 selected;
};
void *ScriptProc_Start(u16, u16);
void Palette_CopyBank(void *, const void *);
void sub_0818171C(struct ActorPaletteOwner *p)
{
    typeof(p->palettes) *palettes = &p->palettes;
    struct ActorPaletteSource *source = ScriptProc_Start(0x92B3, 0xCB8F);
    const void *bank;
    bank = source->banks[0];
    Palette_CopyBank(palettes->banks[p->selected], bank);
    bank = source->banks[1];
    Palette_CopyBank(palettes->banks[p->selected + 1], bank);
    bank = source->banks[2];
    {
        u32 offset = (p->selected << 5) + 0x48;
        u32 address = (u32)palettes;
        address = offset + address;
        Palette_CopyBank((void *)address, bank);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0818171C);
#endif
