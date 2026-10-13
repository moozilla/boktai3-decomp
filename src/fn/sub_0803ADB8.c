// MATCHING_CONCESSION: register-pinning
#include "global.h"

// Tentative result-panel layout: six displayed values settle in three stages.
struct P {
    u8 pad[0x18];
    u32 timer;
    u8 state, entered, a, b, c, d, e, f;
};

extern u16 gUnk_03005260[];
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
int Mod(int, int);
void sub_0822B2F8(int);
void sub_082213B8(void *, void *, int, int, int, int, int, int, int);

static inline int randomValue(u16 *table, int limit)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return Mod(table[gUnk_03005308], limit);
}

void sub_0803ADB8(struct P *p)
{
    int a, b, c, d, e, f;
    u8 *base;
    if (p->entered)
        p->entered = 0;
    if (gUnk_03005260[1] & 0x30F)
        p->timer = 100;
    if (p->timer <= 29) {
        register u16 *table asm("r6") = gUnk_0203B400;
        asm("" : "+r"(table));
        a = randomValue(table, 99);
        d = randomValue(table, 99);
    } else {
        a = p->a;
        d = p->d;
    }
    if (p->timer <= 59) {
        register u16 *table asm("r6") = gUnk_0203B400;
        asm("" : "+r"(table));
        b = randomValue(table, 60);
        e = randomValue(table, 60);
    } else {
        b = p->b;
        e = p->e;
    }
    if (p->timer <= 89) {
        register u16 *table asm("r6") = gUnk_0203B400;
        asm("" : "+r"(table));
        c = randomValue(table, 99);
        f = randomValue(table, 99);
    } else {
        c = p->c;
        f = p->f;
    }
    if (p->timer <= 89 && Mod(p->timer, 2) == 0)
        sub_0822B2F8(0x2A5);
    if (a > 99)
        a = 99;
    if (d > 99)
        d = 99;
    if (b > 59)
        b = 59;
    if (e > 59)
        e = 59;
    if (c > 99)
        c = 99;
    if (f > 99)
        f = 99;
    {
        register u8 *drawing asm("r0") = (u8 *)p + 0x178;
        asm("" : "+r"(drawing));
        base = (u8 *)p + 0x30;
        sub_082213B8(drawing, base, a, 2, 51, 168, 32, 8, 0);
    }
    sub_082213B8((u8 *)p + 0x238, base, b, 2, 51, 192, 32, 8, 0);
    sub_082213B8((u8 *)p + 0x2F8, base, c, 2, 51, 216, 32, 8, 0);
    sub_082213B8((u8 *)p + 0x3B8, base, d, 2, 51, 168, 56, 8, 0);
    sub_082213B8((u8 *)p + 0x478, base, e, 2, 51, 192, 56, 8, 0);
    sub_082213B8((u8 *)p + 0x538, base, f, 2, 51, 216, 56, 8, 0);
    if (p->timer > 99) {
        p->state = 2;
        p->entered = 1;
        p->timer = 0;
    } else {
        p->timer++;
    }
}
