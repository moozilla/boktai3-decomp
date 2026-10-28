#include "global.h"
#include "ram.h"

typedef struct Vec4 {
    s16 x, y, z, w;
} Vec4;
extern u8 gUnk_0203B400[];
void EffectGroup_FadeFirstActive(void *);
void Player_PlayLocalSong(u32, u32);
s32 EffectGroup_SpawnPart(void *, u32, Vec4 *, Vec4 *, Vec4 *, u32, u32);

static inline s32 Random(void)
{
    u8 *table = gUnk_0203B400;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(u16 *)(table + (gRandomIndex << 1));
}

// confidence=low: link player effect tick: fades the group or spawns a randomized particle.
void sub_08239D38(u8 *p)
{
    Vec4 pos;
    Vec4 vel;
    Vec4 par;
    u32 mode = p[0x5b9];
    u16 *src;
    if (mode == 1) {
        EffectGroup_FadeFirstActive(p + 0x5b8);
        return;
    }
    if (mode == 2)
        Player_PlayLocalSong(*(u32 *)(p + 0x18), 0x134);
    else if (mode == 3)
        Player_PlayLocalSong(*(u32 *)(p + 0x18), 0x134);
    src = *(u16 **)(p + 0x5c0);
    pos.x = src[0];
    pos.y = src[1] + 0x80;
    pos.z = src[2];
    vel.x = (Random() & 15) - 7;
    vel.y = (Random() & 31) - 16;
    vel.z = (Random() & 15) - 7;
    par.x = 5;
    par.y = 10;
    par.z = 5;
    EffectGroup_SpawnPart(p + 0x5b8, 3, &pos, &vel, &par, 40, 40);
}
