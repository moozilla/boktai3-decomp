#include "global.h"
int sub_0801B54C(void *);
void sub_0801C69C(void *, void *);
void ResultGame_TrackTarget(void *);
void sub_0824923C(void *, void *);
void Sound_StopSong(int);
void sub_0801B534(void *, void *);
void ResultGame_Finish(void *);
void ResultGame_Update(u8 *p)
{
    if (sub_0801B54C(p)) {
        sub_0801C69C(p, ResultGame_TrackTarget);
        *(int *)(p + 0x68) = 0;
        *(int *)(p + 0x60) = 0;
        *(int *)(p + 0x64) = 32;
        p[0x4a] = 0;
        *(int *)(p + 0xd4) = 0;
        *(int *)(p + 0xd8) = 0;
        *(int *)(p + 0xdc) = 0;
        *(int *)(p + 0xe0) = 0;
        *(int *)(p + 0x6c) = 2;
        if (*(u32 *)(p + 0x50) > 10)
            *(u32 *)(p + 0x50) = 10;
        *(u32 *)(p + 0x54) = *(u32 *)(p + 0x50);
        *(u16 *)(p + 0xa0) = 0;
        *(u16 *)(p + 0xa2) = 0;
        *(u16 *)(p + 0xa4) = 0;
        *(u16 *)(p + 0xa6) = 0;
        p[0xc4] = 0;
    }
    p[0xbe] = 0;
    sub_0824923C(p, *(void **)(p + 0x7a8));
    if (p[0xc4] == 2) {
        Sound_StopSong(0x3ed);
        Sound_StopSong(0x3ee);
        sub_0801B534(p, ResultGame_Finish);
    } else {
        if (*(u16 *)(p + 0xac) == 0)
            p[0xc4] = 1;
        else
            --*(u16 *)(p + 0xac);
        ++*(u32 *)(p + 0x3c);
    }
}
