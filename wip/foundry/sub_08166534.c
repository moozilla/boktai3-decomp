#define MenuDisplay_UpdateDerivedStats sub_08166534
#define MenuDisplay_LoadAdjustedDigits sub_081664A8
#define Background_GetTileCell sub_08163F70
#include "global.h"
#include "player.h"

struct StatDigitAnimation {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x14];
    s16 x, y, z;
    u8 unknown26[0x3A];
};
struct MenuDisplay {
    u8 unknown0[0xA40];
    struct Player *player;
    u8 unknownA44[0x211C];
    struct StatDigitAnimation digits[3][3];
    u8 unknown2EC0[0x19EC];
    u8 baseStats[3];
    u8 unknown48AF;
    s16 additions[3];
    s16 adjustments[3];
};
u16 *sub_08163F70(s32, u32, u32);
void sub_081664A8(s32, s32, void *, void *, void *, void *);
void sub_08166534(struct MenuDisplay *p, s32 x, s32 y)
{
    s32 i = 0;
    do {
        s32 row, n;
        u16 *tile;
        p->baseStats[i] = p->player->derivedStats[i];
        if (p->baseStats[i] > 100)
            p->baseStats[i] = 100;
        row = y + i;
        tile = sub_08163F70(0, x, row);
        n = 2;
        do {
            *tile++ = 0xF001;
        } while (--n >= 0);
        sub_081664A8(p->baseStats[i] + p->additions[i], p->adjustments[i],
                      (u8 *)p + 0x64,
                      (u8 *)p + (i * 0x120 + 0x2B60),
                      (u8 *)p + (i * 0x120 + 0x2BC0),
                      (u8 *)p + (i * 0x120 + 0x2C20));
        {
            struct StatPosition { s16 x, y, z; };
            struct StatPosition *pos;
            struct StatDigitAnimation *anim;
            pos = (void *)((u8 *)p + i * 0x120 + 0x2B80);
            pos->x = x * 8;
            pos->y = row * 8;
            pos->z = 0;
            pos = (void *)((u8 *)p + i * 0x120 + 0x2BE0);
            pos->x = x * 8 + 8;
            pos->y = row * 8;
            pos->z = 0;
            pos = (void *)((u8 *)p + i * 0x120 + 0x2C40);
            pos->x = x * 8 + 16;
            pos->y = row * 8;
            pos->z = 0;
            anim = (void *)((u8 *)p + i * 0x120 + 0x2B60);
            anim->flags &= ~1;
            anim = (void *)((u8 *)p + i * 0x120 + 0x2BC0);
            anim->flags &= ~1;
            anim = (void *)((u8 *)p + i * 0x120 + 0x2C20);
            anim->flags &= ~1;
        }
        i++;
    } while (i <= 2);
}
