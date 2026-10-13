#include "global.h"
struct S {
    u8 a[0xf4];
    void (*cb)(void);
};
void sub_082096E8(void);
void sub_08209704(void);
void Sound_PlaySong(u32);
u32 sub_0820955C(struct S *p)
{
    if (p->cb == sub_082096E8) {
        p->cb = sub_08209704;
        Sound_PlaySong(0x519);
    } else
        p->cb = sub_082096E8;
    return 0;
}
