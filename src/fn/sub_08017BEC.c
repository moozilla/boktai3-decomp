#include "global.h"
struct Weather {
    u8 pad[24];
    u32 timer;
    u8 entered, pad1d[51];
    int bank;
    void *script;
};
extern u8 *gUnk_02000710;
extern u16 gUnk_03004BD8;
void sub_0821AD08(void *, int);
void sub_080174B8(void *, int);
int sub_080177AC(void *, int, int, int);
static inline void setFlag(void)
{
    u32 mask = 4096;
    *(u32 *)(gUnk_02000710 + 0x868) |= mask;
}
int Weather_StormOnset(struct Weather *p)
{
    int mask;
    if (p->entered) {
        p->entered = 0;
        setFlag();
        if (p->script)
            sub_0821AD08(p->script, 0);
    }
    mask = 512;
    gUnk_03004BD8 = mask | gUnk_03004BD8;
    if (p->timer <= 14) {
        if ((p->timer & 3) <= 2)
            gUnk_03004BD8 &= ~mask;
    } else if (p->timer <= 29) {
        if ((p->timer & 3) <= 1)
            gUnk_03004BD8 &= ~mask;
    } else {
        if (!(p->timer & 3))
            gUnk_03004BD8 &= ~mask;
    }
    sub_080177AC(p, 3, 10, 10);
    if (p->timer > 44) {
        sub_080174B8(p, 3);
        return 1;
    } else {
        p->bank = 0;
        p->timer++;
        return 0;
    }
}
