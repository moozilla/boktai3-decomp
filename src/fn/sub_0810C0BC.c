#include "global.h"

struct F246Slot {
    u16 a;
    u16 b;
    u8 pad4[0x5C];
};
struct F246Panel {
    u8 pad0[0x40];
    struct F246Slot slots[7];
};

void sub_0810C0BC(struct F246Panel *p)
{
    s32 i;
    for (i = 0; i <= 6; i++) {
        switch (i) {
            case 0:
                p->slots[i].a = i;
                p->slots[i].b = i;
                break;
            case 1:
                p->slots[i].a = 0x90;
                p->slots[i].b = 0;
                break;
            case 6:
                p->slots[i].a = 8;
                p->slots[i].b = 8;
                break;
            default:
                p->slots[i].a = 0;
                p->slots[i].b = 0;
                break;
        }
    }
}
