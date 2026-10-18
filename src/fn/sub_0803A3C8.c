#include "global.h"
struct Item {
    u8 state, pad1[0xaf];
};
struct Owner {
    u8 pad0[0x5c];
    struct Item items[16];
};
extern void *gUnk_086057DC[];
int Wireless_GetStatus(void);
void sub_08249244(void *, void *, int, void *);
void WirelessGameItems_PublishPendingEffects(void *);
int WirelessGameItems_Update(struct Owner *p)
{
    if (!Wireless_GetStatus()) {
        struct Item *q = p->items;
        int i = 0;
        do {
            sub_08249244(p, q, i, gUnk_086057DC[q->state]);
            i++;
            q++;
        } while (i <= 15);
        WirelessGameItems_PublishPendingEffects(p);
    }
    return 0;
}
