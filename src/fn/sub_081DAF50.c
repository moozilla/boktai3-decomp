#include "global.h"
void Sound_PlaySong(u32);
void Collider_Refresh(u8 *);
void Task_Kill(u8 *);
static inline void frame(u8 *p)
{
    *(u16 *)(p + 0x28) = *(u16 *)(p + 0xb6) >> 2;
}
s32 sub_081DAF50(u8 *p)
{
    u16 *delay = (u16 *)(p + 0xb4);
    if (*delay)
        (*delay)--;
    else {
        u16 *age;
        u32 value;
        frame(p);
        value = *(u16 *)(p + 0xb6);
        age = (u16 *)(p + 0xb6);
        if (value == 0) {
            *(u32 *)(p + 0x18) &= ~1;
            if (*(s16 *)(p + 0xba) == 0)
                Sound_PlaySong(0x195);
        }
        if (*age <= 7 && *(s16 *)(p + 0xb8) == 0)
            Collider_Refresh(p + 0x60);
        (*age)++;
        if (*age > 23)
            Task_Kill(p);
    }
    return 0;
}
