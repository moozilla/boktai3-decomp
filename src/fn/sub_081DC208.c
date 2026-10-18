#include "global.h"
#define PairedFlags_SelectActive sub_081DC208
struct PairedFlags {
    u8 pad0[0x24];
    u32 firstFlags;
    u8 pad28[0x84];
    u32 secondFlags;
    u8 padB0[0x8c];
    u32 selection;
};
void sub_081DC208(struct PairedFlags *p)
{
    if (p->selection == 0) {
        p->firstFlags &= ~1;
        p->secondFlags |= 1;
    } else {
        p->firstFlags |= 1;
        p->secondFlags &= ~1;
    }
}
