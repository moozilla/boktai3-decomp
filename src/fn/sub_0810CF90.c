#include "global.h"

struct F30Slot {
    u16 a;
    u16 b;
    u8 pad4[0x5C];
};
struct F30Panel {
    u8 pad0[0x40];
    struct F30Slot slots[7];
};

void sub_0810CF90(struct F30Panel *p)
{
    s32 i;
    for (i = 0; i <= 6; i++) {
        switch (i) {
            case 5:
                p->slots[i].a = 0;
                p->slots[i].b = 0;
                break;
            case 6:
                p->slots[i].a = 0x90;
                p->slots[i].b = 0;
                break;
            default:
                p->slots[i].a = 0;
                p->slots[i].b = 0;
                break;
        }
    }
}
