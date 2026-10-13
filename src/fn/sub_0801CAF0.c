#include "global.h"
extern u8 gUnk_0824DB2D[];
void Sound_StopSong(int);
void Sound_PlaySong(int);
void ResultMenu_EnqueueMarker(void *);
void ResultGame_ScorePress(u8 *p)
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
    value = *(u16 *)(p + 0xa4);
    if (delta <= limit) {
        int zero;
        value += 5;
        if (value > 65535)
            value = 65535;
        zero = 0;
        *(u16 *)(p + 0xa4) = value;
        ++*(int *)(p + 0xd8);
        p[0xbf] = zero;
        Sound_PlaySong(0x394);
    } else if (delta <= width) {
        value += 3;
        if (value > 65535)
            value = 65535;
        *(u16 *)(p + 0xa4) = value;
        ++*(int *)(p + 0xdc);
        p[0xbf] = 1;
        Sound_PlaySong(0x395);
    } else {
        if (value > 65535)
            value = 65535;
        *(u16 *)(p + 0xa4) = value;
        ++*(int *)(p + 0xe0);
        p[0xbf] = 2;
        Sound_PlaySong(0x396);
    }
    ResultMenu_EnqueueMarker(p);
    ++*(u16 *)(p + 0xa6);
}
