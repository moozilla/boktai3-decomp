#include "global.h"
struct S {
    u8 p0[0x14];
    u8 pad[0x3a4 - 0x14];
    s32 w3a4;
};
void Sound_PlaySong(u32);
void SunOrb_SetUpdateCallback(void *, void (*)(void));
void OrbitEffect_FireVolley(void);
u32 OrbitEffect_UpdateExpansion(struct S *p)
{
    p->w3a4 += 0x4000;
    if (p->w3a4 > 0x40000) {
        Sound_PlaySong(0x564);
        SunOrb_SetUpdateCallback((u8 *)p + 0x14, OrbitEffect_FireVolley);
    }
    return 0;
}
