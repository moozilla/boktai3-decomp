#include "global.h"
/* Finish a delayed controller callback, selecting the time-of-day sound. */
struct S {
    u8 pad[0xf88];
    u8 timer;
    u8 gap[15];
    void (*callback)(void *);
};
extern u8 gUnk_03005430[];
void Sound_PlaySong(u32);
void SunClock_GetBaseHour(void);
void Actor_UpdateInteractionSoundDelay(struct S *p)
{
    if (--p->timer == 0) {
        p->callback = 0;
        if ((u8)(gUnk_03005430[14] - 1) <= 2)
            Sound_PlaySong(0x120);
        else
            SunClock_GetBaseHour();
    }
}
