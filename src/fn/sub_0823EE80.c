#include "global.h"

extern const s16 gUnk_086149C4[];
void Player_PlayLocalSong(u32, u32);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);

struct Ctx0823EE80 {
    u8 pad0[0x9C];
    u16 destX;
    u16 destY;
    u16 destZ;
    u16 padA2;
    u16 originX;
    u16 originY;
    u16 originZ;
    u8 padAC[0xC6 - 0xAA];
    u16 timer;
    u8 heading;
};

static inline s32 scale(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

u32 sub_0823EE80(u8 *p)
{
    struct Ctx0823EE80 *q = (struct Ctx0823EE80 *)(p + 0xB44);
    u16 *timer = &q->timer;
    if (*timer == 0)
        Player_PlayLocalSong(*(u32 *)(p + 0x18), 0x391);
    *timer += 1;
    if (*timer <= 5) {
        q->destX = q->originX + scale(gUnk_086149C4[(p[0xC0C] + 0x40) & 0xFF] * 100);
        q->destY = q->originY;
        q->destZ = q->originZ + scale(gUnk_086149C4[q->heading] * 100);
        Collider_SetPosition(p + 0xB8C, p + 0xBE0, 0);
        Collider_Refresh(p + 0xB8C);
        return 0;
    }
    *timer = 0;
    return 1;
}
