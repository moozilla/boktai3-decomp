#include "player.h"
#include "contact_actor.h"

// confidence=low: holding input charges recoil and release advances the tether attack.

// Remaining differences: charge-duration store and particle bitfield setup instruction order.
extern const s16 gUnk_086149C4[];
void SceneParticleBurst_CreateWhilePaused(void *, void *, void *, u32, u32, u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(u8 *, s32, s32);
void Player_SetTetherMode(u8 *, s32, s32);
void Player_PlayLocalSong(s32 a, u32 b);
s32 Player_UpdateFacingFromInputHistory(u8 *p);
void sub_0823B308(u8 *p);

static inline int scale(int n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

void Player_UpdateChargedTetherStrike(u8 *p)
{
    struct ContactPackedVector ba;
    struct ContactPackedVector bb;
    u16 *t;
    u16 *c;
    u16 *r;
    s32 a, id;

    switch (((struct Player *)p)->tetherPhase) {
        case 0:
            if (Player_SetAnimationIndex(
                    p, ActorVariant_GetTableValue(((struct Player *)p)->actionMode, 0x23), 0x40))
                Player_SetTetherMode(p, 3, 1);
            break;
        case 1:
            t = (u16 *)(p + 0x45A);
            if (*t == 0)
                Player_PlayLocalSong(*(s32 *)(p + 0x18), 0x1F7);
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(
                p, ActorVariant_GetTableValue(((struct Player *)p)->actionMode, 0x24), 0x40);
            (*t)++;
            if (*t <= 6)
                break;
            *(u8 *)(p + 0xC0F) = 0;
            *(u16 *)(p + 0x4D2) = 0;
            if (*(u16 *)(p + 0x428) <= 0x18) {
                a = *(s32 *)(p + 0x18);
                id = 0xD5;
                goto fin;
            }
            *(u16 *)(p + 0xC12) = 0xF;
            if (((struct Player *)p)->inputKeys[0] & 2) {
                Player_SetTetherMode(p, 3, 2);
                break;
            }
            goto l340;
        case 2:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(
                p, ActorVariant_GetTableValue(((struct Player *)p)->actionMode, 0x24), 0x40);
            (*(u16 *)(p + 0xC12))++;
            t = (u16 *)(p + 0x45A);
            if (*t == 6)
                Player_PlayLocalSong(*(s32 *)(p + 0x18), 0x1F8);
            if (!(((struct Player *)p)->inputKeys[0] & 2))
                goto l340;
            (*t)++;
            if (*t <= 0x3B)
                break;
            *(u8 *)(p + 0xC0F) = 1;
            *(u16 *)(p + 0x4D2) = 0x1E;
            {
                u32 off = 0xAD6;
                u8 *q;
                u32 v;
                q = p + off;
                v = 0x16E;
                *(u16 *)q = v;
            }
            {
                u32 off2 = 0xAD8;
                u8 *q2;
                q2 = p + off2;
                *(u16 *)q2 = 0x24;
            }
            Player_PlayLocalSong(*(s32 *)(p + 0x18), 0x1F9);
            Player_SetTetherMode(p, 3, 4);
            break;
        case 4:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(
                p, ActorVariant_GetTableValue(((struct Player *)p)->actionMode, 0x24), 0x40);
            (*(u16 *)(p + 0xC12))++;
            if (((struct Player *)p)->inputKeys[0] & 2)
                break;
        l340:
            a = *(s32 *)(p + 0x18);
            id = 0xCD;
        fin:
            Player_PlayLocalSong(a, id);
            Player_SetTetherMode(p, 3, 5);
            break;
        case 5: {
            s32 ang, n;
            Player_SetAnimationIndex(
                p, ActorVariant_GetTableValue(((struct Player *)p)->actionMode, 0x25), 0x40);
            if (((struct Player *)p)->inputKeys[1] & 2)
                *(u8 *)(p + 0xBF6) = 1;
            r = (u16 *)(p + 0x4D2);
            if (*r != 0) {
                if (*r > 5) {
                    ba.x = 0;
                    ba.y = 0;
                    ba.z = 0;
                    bb.x = 32;
                    bb.y = 32;
                    bb.z = 32;
                    SceneParticleBurst_CreateWhilePaused(p + 0x30, &ba, &bb, 1, 3, 4);
                }
                ang = ((*(u8 *)(p + 0x3A4) + 5) & 7) << 5;
                n = *r;
                *(s16 *)(p + 0x38) = -scale(gUnk_086149C4[(ang + 0x40) & 0xFF] * n);
                *(s16 *)(p + 0x3C) = -scale(*(u16 *)(p + 0x4D2) * gUnk_086149C4[ang]);
                *(u16 *)(p + 0x4D2) = (*(u16 *)(p + 0x4D2) * 3) >> 2;
            } else {
                (((struct Player *)p)->tetherFrame)++;
                if (((struct Player *)p)->tetherFrame > 4)
                    Player_SetTetherMode(p, 3, 6);
            }
            break;
        }
        case 6:
            Player_SetAnimationIndex(
                p, ActorVariant_GetTableValue(((struct Player *)p)->actionMode, 0x24), 0x40);
            if (((struct Player *)p)->inputKeys[1] & 2)
                *(u8 *)(p + 0xBF6) = 1;
            {
                u32 released = *(u8 *)(p + 0xBF6);
                if (released) {
                    *(u8 *)(p + 0xBF6) = 0;
                    ((struct Player *)p)->tetherPhase = 1;
                    break;
                }
                (((struct Player *)p)->tetherFrame)++;
                if (((struct Player *)p)->tetherFrame > 0xE) {
                    *(u8 *)(p + 0xBF6) = 0;
                    Player_SetTetherMode(p, 3, 7);
                }
            }
            break;
        case 7:
            Player_SetAnimationIndex(
                p, ActorVariant_GetTableValue(((struct Player *)p)->actionMode, 0x23), 0x40);
            (((struct Player *)p)->tetherFrame)++;
            if (((struct Player *)p)->tetherFrame > 4)
                Player_SetTetherMode(p, 0, 0);
            break;
        default:
            break;
    }
    c = (u16 *)(p + 0xC12);
    if (*c > 0xE) {
        if (((struct Player *)p)->actionMode == 1)
            sub_0823B308(p);
        *c = 0;
    }
    *(u8 *)(p + 0xAC9) = 8;
}
