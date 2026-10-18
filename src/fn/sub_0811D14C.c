#include "global.h"

struct Unk_0811D14C {
    u8 filler0[0x20];
    u16 unk20;
};

void ModifierMenu_InitSelectionSnapshot(struct Unk_0811D14C *dest, u8 *src)
{
    CpuSet(src + 0x20, dest, 0x04000008);
    dest->unk20 = 0;
}
