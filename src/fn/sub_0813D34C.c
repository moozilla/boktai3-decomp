#include "global.h"

struct O0813D34C { u8 f0[6]; u16 f6; u8 f8[4]; u32 fC; };
struct P0813D34C {
    u8 f0[0xC6]; u16 fC6; u8 fC8[0xC]; u32 fD4;
    u8 fD8[0x178 - 0xD8]; struct O0813D34C *f178;
    u8 f17C[0xAC2 - 0x17C]; u16 fAC2; u16 fAC4; u8 fAC6[2]; u8 fAC8;
};
void sub_0813D150(void);
void sub_0813D178(void *);
void sub_0813CB8C(void *, u32);

void sub_0813D34C(struct P0813D34C *p)
{
    sub_0813D150();
    sub_0813D178(p);
    p->fAC4 = 0xFFFF;
    p->fAC8 = 0xFF;
    p->fC6 = p->fAC2;
    p->fD4 = (u32)p + 0x360;
    p->f178->f6 = p->fAC2;
    p->f178->fC = (u32)p + 0x360;
    sub_0813CB8C(p, 0);
}
