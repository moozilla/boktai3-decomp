#include "global.h"
struct S {
    u8 a[0xf4];
    void (*cb)(void);
};
void FanCannon_UpdateInactive(void);
void sub_08209704(void);
void Sound_PlaySong(u32);
u32 FanCannon_Activate(struct S *p)
{
    if (p->cb == FanCannon_UpdateInactive) {
        p->cb = sub_08209704;
        Sound_PlaySong(0x519);
    }
}
