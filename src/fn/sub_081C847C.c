#include "global.h"
struct S {
    u8 f[4];
    u8 g[3];
    u8 b7;
    u8 b8;
    u8 b9;
    u8 h[0x16];
    u32 a20;
    u32 a24;
};
struct P {
    u32 a;
    u32 b;
};
void SunOrb_GetOrCreateManager(void);
u32 SunOrb_CreatePlayerTargeted(void *);
void Sound_PlaySong(u32);
void StateActor_SpawnRaisedEffect(u32 a, struct S *p)
{
    u8 *e = (u8 *)p + 0x20;
    if (p->b9 != 0) {
        struct P *d;
        u16 *h;
        p->b9 = 0;
        p->b7 = 0;
        SunOrb_GetOrCreateManager();
        d = (struct P *)((u8 *)p + 0x54);
        *d = *(struct P *)((u8 *)p + 0x68);
        h = (u16 *)((u8 *)p + 0x56);
        *h = (u16)(0x100 + *h);
        *(u32 *)(e + 4) = SunOrb_CreatePlayerTargeted(d);
        Sound_PlaySong(0x571);
    }
    p->b7 = 1;
}
