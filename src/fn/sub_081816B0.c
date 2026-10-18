#define ActorPalette_RestoreSelectedBanks sub_081816B0
#include "global.h"
struct ActorPaletteBanks {
    u32 callbacks[2];
    u16 original[16][16];
    u16 working[16][16];
    u8 unknown408[0x82];
    u8 selected, alternate;
};
void Palette_CopyBank(void *, const void *);
void sub_081816B0(struct ActorPaletteBanks *p)
{
    Palette_CopyBank(p->working[p->selected], p->original[p->selected]);
    Palette_CopyBank(p->working[p->selected + 1], p->original[p->selected + 1]);
    Palette_CopyBank(p->working[p->selected + 2], p->original[p->selected + 2]);
    Palette_CopyBank(p->working[p->alternate], p->original[p->alternate]);
}
