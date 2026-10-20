#include "global.h"

#ifdef NONMATCHING
// fuzzy: 89.9%
// differs: register allocation of the facing-byte block at the top

extern const s16 gUnk_086149C4[];
s32 ActorVariant_GetTableValue(u32, s32);
s32 sub_08238280(u8 *, s32, s32);
void sub_08238238(u8 *);
void Player_SetTetherMode(u8 *, s32, s32);
void Player_PlayLocalSong(s32 a, u32 b);
void Camera_RequestShake(u32);
void RisingSprite_Spawn(u8 *);

static inline int scale12(int n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

static inline int scale3(int n)
{
    return n >= 0 ? n >> 3 : -((-n) >> 3);
}

#define Player_UpdateTetherRecoil sub_08240200

void Player_UpdateTetherRecoil(u8 *p)
{
    u16 *r;
    u16 *c;
    u8 *ap;
    u32 v;

    if (*(u8 *)(p + 0x457) != 0x1D) {
        Player_SetTetherMode(p, 0x1D, 0);
        ap = p + 0x3A4;
        v = *ap;
        if ((v & 0xFF) > 4) {
            *(u8 *)(p + 0x3A2) = (8 - *ap) >> 1;
            *(u8 *)(p + 0x3A3) = 1;
        } else {
            *(u8 *)(p + 0x3A2) = (v & 0xFF) >> 1;
            *(u8 *)(p + 0x3A3) = 0;
        }
        sub_08238280(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x29), 0x40);
        r = (u16 *)(p + 0x4C2);
        if (*r != 0) {
            Player_PlayLocalSong(*(s32 *)(p + 0x18), *r);
            *r = 0;
        }
    }
    r = (u16 *)(p + 0x270);
    if (*r != 0) {
        if (*(u8 *)(p + 0x479) != 0) {
            s16 *t = *(s16 **)(p + 0x47C);
            int dx = t[0] - *(s16 *)(p + 0x30);
            int dy = t[2] - *(s16 *)(p + 0x34);
            *(s16 *)(p + 0x38) = scale3(dx);
            *(s16 *)(p + 0x3C) = scale3(dy);
            c = (u16 *)(p + 0x45A);
            if (*c <= 5) {
                if (!(*c & 1)) {
                    Player_PlayLocalSong(*(s32 *)(p + 0x18), 0x191);
                    RisingSprite_Spawn(p + 0x30);
                }
                (*c)++;
            }
        } else {
            int ang = ((*(u8 *)(p + 0x3A4) + 5) & 7) << 5;
            *(s16 *)(p + 0x38) = -scale12(gUnk_086149C4[(ang + 0x40) & 0xFF] * *r);
            *(s16 *)(p + 0x3C) = -scale12(gUnk_086149C4[ang] * *(u16 *)(p + 0x270));
        }
        Camera_RequestShake(*(u16 *)(p + 0x270));
        *(u16 *)(p + 0x270) = (*(u16 *)(p + 0x270) * 3) >> 2;
    } else {
        *r = 0;
        *(u8 *)(p + 0x479) = 0;
        if (*(u8 *)(p + 0x5B9) == 3 && *(u8 *)(p + 0x5BB) != 0) {
            Player_SetTetherMode(p, 0x16, 0);
        } else if (sub_08238280(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x2A), 0x40)) {
            sub_08238238(p);
            Player_SetTetherMode(p, 0, 0);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08240200);
#endif
