#include "global.h"
#include "game_state.h"

struct Vec08194CB8 {
    s16 x, y, z, w;
};
extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
void Sound_PlaySongChecked(u32);

static inline s32 Random2(void)
{
    gUnk_03005304 = (gUnk_03005304 + 1) & 1023;
    return gUnk_086141BC[gUnk_03005304];
}

void sub_08194CB8(struct Vec08194CB8 *pos)
{
    struct Vec08194CB8 d;
    d.x = pos->x - gUnk_02000710->positionX;
    d.z = pos->z - gUnk_02000710->positionZ;
    if (d.x < 0)
        d.x = -d.x;
    if (d.z < 0)
        d.z = -d.z;
    if (d.x <= 0x900 && d.z <= 0xc00) {
        if (Random2() > 0x7fff)
            Sound_PlaySongChecked(0x1c0);
        else
            Sound_PlaySongChecked(0x1bf);
    }
}
