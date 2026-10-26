#include "global.h"
#include "engine.h"
struct Pair {
    u32 a, b;
};
void SolarBomb_InitSprite(u8 *);
void sub_08234138(u8 *);
s32 Div(s32, s32);
void Sound_PlaySong(u32);
void sub_08233B78(u8 *);
void SolarBomb_SetState(u8 *, void (*)(u8 *));
static inline void start(u16 *dst, u32 duration)
{
    *dst = duration;
    Sound_PlaySong(duration + 21);
}
#define H(o) (*(u16 *)(p + (o)))
s32 SolarBomb_Init(u8 *p, u8 *owner, struct Pair *v, u16 mode)
{
    *(struct Pair *)(p + 0x64) = *v;
    *(u8 **)(p + 0x18) = owner;
    H(0x1d0) = mode;
    SolarBomb_InitSprite(p);
    sub_08234138(p);
    (*(u8 **)(p + 0x18))[0x4ef]++;
    if (H(0x1d0))
        H(0x1d4) = 120;
    else if (gEngineFlags & 2048)
        H(0x1d4) = 90;
    else
        H(0x1d4) = 180;
    H(0x1d2) = Div(H(0x1d4), 3);
    start((u16 *)(p + 0x1d6), 900);
    SolarBomb_SetState(p, sub_08233B78);
    return 0;
}
