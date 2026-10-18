#include "global.h"
extern const u32 gUnk_08612008[];
void sub_0824923C(void *, u32);
void Vanargand_RunSecondaryModeCallback(u8 *p)
{
    sub_0824923C(p, gUnk_08612008[p[0xAD]]);
}
