#include "global.h"
struct P {
    u8 f0[0x33C];
    u16 a;
    u16 b;
    u8 f1[1];
    u8 c;
    u8 f2[4];
    u16 d;
};
void Sound_StopAllTrackedSongs(void);
void Sound_PlaySong(s32);
void PlayerBurstEffect_InitExpansion(struct P *p)
{
    Sound_StopAllTrackedSongs();
    Sound_PlaySong(2);
    Sound_PlaySong(0x12c);
    p->a = 10;
    p->b = 0x40;
    p->c = 1;
}
