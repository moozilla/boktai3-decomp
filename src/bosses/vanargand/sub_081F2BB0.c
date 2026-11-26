#include "global.h"

void Sound_PlaySong(u32);
void Task_Kill(void *);

// confidence=low: ExplosionEffect_Update without the start delay or collider: fades in by age,
// plays sound 0x195 on the first frame, dies after 24 frames and sinks by the speed at 0x64.
s32 sub_081F2BB0(u8 *p)
{
    u16 *age = (u16 *)(p + 0x60);
    *(u16 *)(p + 0x28) = *age >> 2;
    if (*age == 0) {
        *(u32 *)(p + 0x18) &= ~1;
        Sound_PlaySong(0x195);
    }
    (*age)++;
    if (*age > 23)
        Task_Kill(p);
    *(u16 *)(p + 0x34) -= *(u32 *)(p + 0x64);
    return 0;
}
