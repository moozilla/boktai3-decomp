#include "global.h"
#include "area_effect_link.h"

s32 sub_0823C4B4(u8 *);
s32 LinkPlayer_HasSlotValueAtLeast(u8 *, s32);
void Player_PlayLocalSong(s32, u32);
void Player_SetTetherMode(u8 *, s32, s32);
s32 Div(s32, s32);
void Player_SetDirectionalVelocity_08238300(u8 *, s32);
void Player_PlayActionCueSound(u8 *, u32);
void sub_08238238(u8 *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(u8 *, s32, s32);

void sub_0823C760(u8 *p)
{
    int c;
    if (p[0x457] != 2) {
        if (LinkPlayer_HasSlotValueAtLeast(p, 1)) {
            s32 slot;
            u8 *base;
            s32 v;
            Player_PlayLocalSong(*(s32 *)(p + 0x18), 0x392);
            slot = *(s32 *)(p + 0x18);
            base = gLinkSession;
            if (base == 0 || slot < 0)
                v = 0;
            else {
                base += 0x5C;
                v = base[slot];
            }
            v = v * 2 + 14;
            p[0x4F0] = v;
            if ((u8)v > 0x20)
                p[0x4F0] = 0x20;
            p[0x4F1] = Div(0x200, p[0x4F0]);
            Player_SetTetherMode(p, 2, 0);
        } else {
            Player_PlayLocalSong(*(s32 *)(p + 0x18), 0x192);
            Player_SetTetherMode(p, 2, 1);
        }
    }
    switch (p[0x458]) {
        case 0: {
            u16 *t = (u16 *)(p + 0x45A);
            u32 lim;
            s32 n = ++*t;
            lim = p[0x4F1];
            if ((u16)n >= lim) {
                Player_SetTetherMode(p, 0, 0);
                break;
            }
            c = 0x172;
            if (*t < (s32)lim - 8) {
                *(u16 *)(p + 0xAD6) = c;
                *(u16 *)(p + 0xAD8) = 0x20;
            }
            if (*(s16 *)(p + 0x34C) != -1 && *(s16 *)(p + 0x4B6) == 0)
                p[0x3A4] = sub_0823C4B4(p);
            Player_SetDirectionalVelocity_08238300(p, p[0x4F0]);
            Player_PlayActionCueSound(p, 1);
            sub_08238238(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p[0x418], 1), 0x20);
            break;
        }
        case 1: {
            u16 *t;
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p[0x418], 0), 0x40);
            t = (u16 *)(p + 0x45A);
            (*t)++;
            if (*t > 7)
                Player_SetTetherMode(p, 0, 0);
            break;
        }
    }
}
