#include "global.h"
void Sound_PlaySong(u32);
s32 Collider_Refresh(u8 *);
void Task_Kill(u8 *);
s32 sub_082331E4(u8 *p)
{
    u16 *delay = (u16 *)(p + 0xb4);
    if (*delay)
        (*delay)--;
    else {
        u16 *age = (u16 *)(p + 0xb6);
        *(u16 *)(p + 0x28) = *age >> 2;
        if (*age == 0) {
            *(u32 *)(p + 0x18) &= ~1;
            Sound_PlaySong(0x195);
        }
        if (*age <= 7)
            Collider_Refresh(p + 0x60);
        (*age)++;
        if (*age > 23)
            Task_Kill(p);
    }
    return 0;
}
