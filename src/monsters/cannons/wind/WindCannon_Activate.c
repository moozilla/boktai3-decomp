#include "global.h"
struct S {
    u8 a[0xf4];
    void (*cb)(void);
};
void WindCannon_UpdateInactive(void);
void WindCannon_UpdateActive(void);
void Sound_PlaySong(u32);
u32 WindCannon_Activate(struct S *p)
{
    if (p->cb == WindCannon_UpdateInactive) {
        p->cb = WindCannon_UpdateActive;
        Sound_PlaySong(0x519);
    }
}
