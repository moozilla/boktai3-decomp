#include "global.h"

void sub_08022AF4(void *);

struct S {
    u8 pad0[0xAE];
    u8 f_ae;
    u8 pad1;
    u8 f_b0;
    u8 pad2[0xC4 - 0xB1];
    s32 f_c4;
    u8 pad3[0xE1 - 0xC8];
    u8 f_e1;
    u8 pad4[0x358 - 0xE2];
    s32 f_358;
    u8 pad5[0x394 - 0x35C];
    u8 f_394;
    u8 pad6[0x3B2 - 0x395];
    u16 f_3b2;
    u8 pad7[0x3B9 - 0x3B4];
    u8 f_3b9;
};

static inline void setState(struct S *p, s32 v)
{
    p->f_ae = v;
    p->f_c4 = 0;
    p->f_b0 = 1;
}

void sub_08022DBC(struct S *p)
{
    s32 r;
    s32 e;
    struct S *q = p;

    if (p->f_358 == 3) {
        if (p->f_3b9 != 0) {
            r = 0xE;
            goto done;
        }
    } else {
        s32 b = p->f_3b9;
        if (b == 3) {
            sub_08022AF4(p);
        } else if (b != 0) {
            r = 0xE;
            goto done;
        }
    }
    switch (q->f_3b2) {
    case 0:
        r = 9;
        break;
    case 1:
        r = 10;
        break;
    case 2:
        if (q->f_358 != 3 && (e = p->f_e1) <= 0x3F)
            q->f_394 = 1;
        else
            q->f_394 = 0;
        r = 2;
        break;
    case 3:
        r = 11;
        break;
    case 4:
        if ((e = p->f_e1) <= 0x3F)
            r = 14;
        else
            r = 11;
        break;
    case 5:
        r = 15;
        break;
    case 6:
        r = 16;
        break;
    default:
        return;
    }
done:
    setState(p, r);
}
