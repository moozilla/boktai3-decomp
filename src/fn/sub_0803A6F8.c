#include "global.h"
struct Item {
    u8 state, entered, unused, count;
    u8 pad4[4];
    u16 timer;
    u8 pada[0xa6];
};
struct Owner {
    u8 pad0[0x1c];
    u8 counts[4];
    u8 pad20[0x3c];
    struct Item items[16];
};
extern struct Owner *gUnk_02000100;
void WirelessGameItems_StartOutgoingItems(void)
{
    struct Owner *p = gUnk_02000100;
    if (p) {
        int i = 0;
        int state = 4;
        int zero = 0;
        u8 *count = p->counts;
        do {
            if (*count) {
                int value;
                int offset = i * 0xb0 + 0x5c;
                struct Item *q;
                q = (struct Item *)(((u8 *)p) + (*(&offset)));
                q->state = state;
                do {
                    q->entered = 1;
                    q->timer = zero;
                } while (0);
                value = *count;
                {
                    int clear = 0;
                    if (value > 10) {
                        value = 10;
                    }
                    q->count = value;
                    q->unused = clear;
                }
                *count = zero;
            }
            count++;
            i++;
        } while (i <= 3);
    }
}
