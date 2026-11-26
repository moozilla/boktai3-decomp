#include "global.h"
struct S {
    u8 a[0xf4];
    void (*cb)(void);
};
void FanCannon_UpdateInactive(void);
void FanCannon_UpdateActive(void);
void Sound_PlaySong(u32);
u32 FanCannon_ToggleActiveState(struct S *p)
{
    if (p->cb == FanCannon_UpdateInactive) {
        p->cb = FanCannon_UpdateActive;
        Sound_PlaySong(0x519);
    } else
        p->cb = FanCannon_UpdateInactive;
    return 0;
}
