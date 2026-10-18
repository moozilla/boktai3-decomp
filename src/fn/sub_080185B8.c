#include "global.h"
s32 Palette_IsDirty(void);
void PaletteCycle_RefreshBasePalette(u8 *);
void sub_08018454(u8 *, u8 *);
u32 PaletteCycle_Update(u8 *p)
{
    u32 i;
    u8 *e;
    if (Palette_IsDirty() != 0)
        PaletteCycle_RefreshBasePalette(p);
    i = 0;
    if (i < *(u32 *)(p + 0x18)) {
        e = p + 0x21c;
        do {
            sub_08018454(p, e);
            e += 0x18;
            i++;
        } while (i < *(u32 *)(p + 0x18));
    }
    return 0;
}
