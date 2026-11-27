#include "global.h"
extern u8 gUnk_0824DB2D[];
void Sound_StopSong(int);
void Sound_PlaySong(int);
void ResultMenu_EnqueueMarker(void *);
void ResultGame_ScoreAlignment(u8 *p)
{
    int delta = *(int *)(p + 0x8c) - *(int *)(p + 0x9c);
    int limit, value, width;
    if (delta < 0)
        delta = -delta;
    {
        int enabled = p[0xbc];
        limit = 96;
        if (enabled)
            limit = 160;
    }
    width = gUnk_0824DB2D[p[0xae]] << 5;
    value = *(u16 *)(p + 0xa0);
    if (delta <= limit) {
        value += 5;
        if (value > 65535)
            value = 65535;
        *(u16 *)(p + 0xa0) = value;
        ++*(int *)(p + 0xd8);
        if (p[0xbf]) {
            Sound_StopSong(0x3ee);
            Sound_PlaySong(0x3ed);
        }
        p[0xbf] = 0;
    } else if (delta <= width) {
        value += 3;
        if (value > 65535)
            value = 65535;
        *(u16 *)(p + 0xa0) = value;
        ++*(int *)(p + 0xdc);
        if (delta > 320)
            Sound_StopSong(0x3ed);
        if (p[0xbf] != 1)
            Sound_StopSong(0x3ee);
        p[0xbf] = 1;
    } else {
        if (value > 65535)
            value = 65535;
        *(u16 *)(p + 0xa0) = value;
        ++*(int *)(p + 0xe0);
        if (p[0xbf] != 2) {
            Sound_StopSong(0x3ed);
            Sound_PlaySong(0x3ee);
        }
        p[0xbf] = 2;
    }
    ResultMenu_EnqueueMarker(p);
    ++*(u16 *)(p + 0xa2);
}
