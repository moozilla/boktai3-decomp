#include "global.h"

struct F3CSlot {
    u16 a;
    u16 b;
    u8 pad4[0x5C];
};
struct F3CPanel {
    u8 pad0[0x40];
    struct F3CSlot slots[10];
};

void sub_0810DC7C(struct F3CPanel *p)
{
    s32 i;
    for (i = 0; i <= 9; i++) {
        switch (i) {
            case 2:
                p->slots[i].a = 0;
                p->slots[i].b = -24;
                break;
            case 7:
                p->slots[i].a = 0x60;
                p->slots[i].b = 0x10;
                break;
            case 8:
                p->slots[i].a = 0x60;
                p->slots[i].b = 0x10;
                break;
            case 9:
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
