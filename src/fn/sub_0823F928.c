#include "global.h"

#ifdef NONMATCHING
// fuzzy: 87.8%
// draft; cases 0-7 follow Player_UpdateTetherChargeB, case 8 is the orbiting hit effect
extern const s16 gUnk_086149C4[];
void SceneParticleBurst_CreateWithPause(void *, void *, void *, u32, u32, u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 sub_08238280(u8 *, s32, s32);
void Player_SetTetherMode(u8 *, s32, s32);
void Player_PlayLocalSong(s32 a, u32 b);
void Player_StopLocalSong(s32 a, u32 b);
s32 Player_UpdateFacingFromInputHistory(u8 *p);
void Player_SetDirectionalVelocity_08238300(u8 *, s32);
void DirectionalEffect_PlaceSprite(u8 *, u32, u8 *);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);
void sub_08238238(u8 *);
u32 Player_GetConfiguredActionPower(u8 *);
void Collider_SetAttackProperties(u8 *, u32, u32, u32, u32, u32);

struct BurstA {
    s32 a;
    u32 b : 16;
    u32 c : 16;
};
struct BurstB {
    u32 a : 16;
    u32 b : 16;
    u32 c : 16;
    u32 d : 16;
};
struct Pos {
    u32 x : 16;
    s32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};

static inline int scale(int n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

void Player_UpdateTetherChargeC(u8 *p)
{
    struct BurstA ba;
    struct BurstB bb;
    struct Pos pos;
    u16 *t;
    u16 *c;
    u8 *q, *pp;
    u32 cnt;
    s32 a, id, ang;

    if (*(u8 *)(p + 0x457) != 3)
        Player_SetTetherMode(p, 3, 0);
    switch (*(u8 *)(p + 0x458)) {
        case 0:
            if (sub_08238280(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x23), 0x40))
                Player_SetTetherMode(p, 3, 1);
            break;
        case 1:
            Player_UpdateFacingFromInputHistory(p);
            sub_08238280(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x24), 0x40);
            (*(u16 *)(p + 0x45A))++;
            if (*(u16 *)(p + 0x45A) <= 6)
                break;
            *(u8 *)(p + 0xC0F) = 0;
            if (*(u16 *)(p + 0x428) <= 9) {
                a = *(s32 *)(p + 0x18);
                id = 0xD5;
                goto fin;
            }
            if (**(u16 **)(p + 0x348) & 2) {
                Player_SetTetherMode(p, 3, 2);
                break;
            }
            goto l203;
        case 2:
            Player_UpdateFacingFromInputHistory(p);
            sub_08238280(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x24), 0x40);
            t = (u16 *)(p + 0x45A);
            if (*t == 6)
                Player_PlayLocalSong(*(s32 *)(p + 0x18), 0x1F8);
            if (!(**(u16 **)(p + 0x348) & 2)) {
            l203:
                a = *(s32 *)(p + 0x18);
                id = 0x203;
            fin:
                Player_PlayLocalSong(a, id);
                Player_SetTetherMode(p, 3, 5);
                break;
            }
            (*t)++;
            if (*t <= 0x1D)
                break;
            *(u8 *)(p + 0xC0F) = 1;
            *(u16 *)(p + 0xAD6) = 0x16F;
            *(u16 *)(p + 0xAD8) = 0x24;
            Player_PlayLocalSong(*(s32 *)(p + 0x18), 0x1F9);
            Player_SetTetherMode(p, 3, 4);
            break;
        case 4:
            Player_UpdateFacingFromInputHistory(p);
            sub_08238280(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x24), 0x40);
            if (**(u16 **)(p + 0x348) & 2)
                break;
            Player_PlayLocalSong(*(s32 *)(p + 0x18), 0x2E9);
            *(u16 *)(p + 0xC0A) = ((8 - *(u8 *)(p + 0x3A4)) & 7) << 5;
            Collider_SetAttackProperties(p + 0xB8C, Player_GetConfiguredActionPower(p), 0x20, 0x10,
                                         2, 0x1E);
            Player_PlayLocalSong(*(s32 *)(p + 0x18), 0x202);
            Player_SetTetherMode(p, 3, 8);
            break;
        case 5:
            sub_08238280(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x25), 0x40);
            if ((*(u16 **)(p + 0x348))[1] & 2)
                *(u8 *)(p + 0xBF6) = 1;
            (*(u16 *)(p + 0x45A))++;
            if (*(u16 *)(p + 0x45A) > 4)
                Player_SetTetherMode(p, 3, 6);
            break;
        case 6:
            sub_08238280(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x24), 0x40);
            if ((*(u16 **)(p + 0x348))[1] & 2)
                *(u8 *)(p + 0xBF6) = 1;
            if (*(u8 *)(p + 0xBF6)) {
                *(u8 *)(p + 0xBF6) = 0;
                *(u8 *)(p + 0x458) = 1;
                break;
            }
            (*(u16 *)(p + 0x45A))++;
            if (*(u16 *)(p + 0x45A) > 0xE) {
                *(u8 *)(p + 0xBF6) = 0;
                Player_SetTetherMode(p, 3, 7);
            }
            break;
        case 7:
            sub_08238280(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x23), 0x40);
            (*(u16 *)(p + 0x45A))++;
            if (*(u16 *)(p + 0x45A) > 4)
                Player_SetTetherMode(p, 0, 0);
            break;
        case 8: {
            *(u8 *)(p + 0x4B9) = 0;
            if (Player_UpdateFacingFromInputHistory(p))
                Player_SetDirectionalVelocity_08238300(p, 6);
            cnt = *(u16 *)(p + 0x45A) & 3;
            pp = p + 0x30;
            if (cnt == 0) {
                ba.a = 0;
                ba.b = 0;
                bb.a = 0x20;
                bb.b = 0;
                bb.c = 0x20;
                SceneParticleBurst_CreateWithPause(pp, &ba, &bb, 1, 2, 4);
            }
            c = (u16 *)(p + 0xC0A);
            *c = (*c + 0x10) & 0xFF;
            DirectionalEffect_PlaceSprite(p, *c, pp);
            *(long long *)&pos = *(long long *)(p + 0x30);
            ang = (0x1A0 - *c) & 0xFF;
            pos.x += scale(gUnk_086149C4[(ang + 0x40) & 0xFF] * 0xAA);
            pos.y += 0xE6;
            pos.z += scale(gUnk_086149C4[ang] * 0xAA);
            q = p + 0xB8C;
            Collider_SetPosition(q, &pos, 0);
            *(u8 *)(p + 0xBCE) = ang;
            Collider_Refresh(q);
            *(u8 *)(p + 0x3A4) = ((0x10F - *(u16 *)(p + 0xC0A)) & 0xFF) >> 5;
            sub_08238238(p);
            sub_08238280(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x25), 0x40);
            t = (u16 *)(p + 0x45A);
            if ((*t & 0xF) == 0)
                Player_PlayLocalSong(*(s32 *)(p + 0x18), 0x204);
            (*t)++;
            if (*t >= (*(u8 *)(p + 0xC0F) << 5)) {
                Player_StopLocalSong(*(s32 *)(p + 0x18), 0x202);
                Player_SetTetherMode(p, 3, 6);
            }
            break;
        }
        default:
            break;
    }
    *(u8 *)(p + 0xAC9) = 9;
}
#else
INCLUDE_ASM("asm/nonmatching", Player_UpdateTetherChargeC);
#endif
