#include "global.h"
struct E {
    u32 a;
    u16 *b;
};
struct P {
    u8 f[0x4c];
    u32 w;
    u8 g[0x7249 - 0x50];
    u8 s;
};
s32 MessageQueue_GetMessagesById(u32, struct E **);
void Nidhoggr_SetBehavior(void *, s32);
void Nidhoggr_CheckActivationTriggers(struct P *p)
{
    struct E *e;
    s32 n = MessageQueue_GetMessagesById(p->w, &e);
    if (n > 0) {
        do {
            if (*e->b == 0 && p->s == 0)
                Nidhoggr_SetBehavior(p, 1);
            n--;
            e++;
        } while (n > 0);
    }
}
