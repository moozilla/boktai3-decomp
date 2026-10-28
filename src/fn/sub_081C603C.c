#include "global.h"
#include "input.h"
struct S {
    u8 f[0x60];
    u32 fl;
    u8 g[0x370 - 0x64];
    s32 cnt;
    u8 h[0x358 - 0x374 + 0x100];
};
struct T {
    u8 f[0x60];
    u32 fl;
    u8 g[0x358 - 0x64];
    s32 st;
    u8 h[0x370 - 0x35c];
    s32 cnt;
};
void Sound_PlaySong(u32);
void ChallengeResult_SetState(void *, void (*)(void *));
void TimeResult_UpdateInput(void *);
void TimeResult_UpdateResultHold(struct T *p)
{
    p->cnt++;
    if (p->cnt > 0x3c || (gPadState[1] & 1) != 0) {
        Sound_PlaySong(0x167);
        if ((u32)(p->st - 1) <= 4)
            p->fl &= ~1;
        ChallengeResult_SetState(p, TimeResult_UpdateInput);
    }
}
