#include "global.h"
struct S {
    u16 h0;
    u16 h2;
    u8 p4[2];
    u8 b6;
    u8 b7;
    u32 w8;
    u8 pc[4];
    u16 h10;
    u16 h12;
    u16 h14;
    u8 p16[2];
    u16 h18;
    u16 h1a;
    u16 h1c;
};
void Memory_Clear(void *, u32);
void PlayerClone_SetBehaviorCallback(void *, void (*)(void));
void PlayerClone_UpdateBehavior(void);
void PlayerClone_InitConfig(struct S *p)
{
    Memory_Clear(p, 0x20);
    PlayerClone_SetBehaviorCallback(p, PlayerClone_UpdateBehavior);
    p->h0 = 0;
    p->h2 = 0x3c;
    p->w8 = 0x10;
    p->b6 = 0;
    {
        u32 c = 0x1e;
        p->h10 = c;
        p->h12 = c;
        p->h14 = c;
        p->h18 = c;
        p->h1a = 0xf;
        p->h1c = c;
    }
}
