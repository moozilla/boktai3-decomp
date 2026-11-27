#include "global.h"

void Sound_PlaySongChecked(u32);
void Armor_UpdatePartLunge(void);

void Armor_UpdatePartLungeDelay(u8 *p)
{
    u32 c = *(u16 *)(p + 0x1D6) - 1;
    *(u16 *)(p + 0x1D6) = c;
    if ((c << 16) == 0) {
        *(u32 *)(p + 0x1D0) = (u32)Armor_UpdatePartLunge;
        Sound_PlaySongChecked(0x16A);
    }
}
