#include "global.h"

/* tentative: drifts the player along a direction while global modifier 08 is set */
#define Player_UpdateModifier08Drift sub_0823BE50

struct Obj {
    u8 pad0[0x18];
    s32 slot;
    u8 pad1C[0x284 - 0x1C];
    u8 flags284;
    u8 pad285[0x292 - 0x285];
    u16 modifier;
    u8 pad294[0x34C - 0x294];
    s16 v34C;
    u8 pad34E[0x3A4 - 0x34E];
    u8 facing;
    u8 pad3A5[0x457 - 0x3A5];
    u8 state457;
    u8 pad458[0x48C - 0x458];
    u8 active;
    u8 played;
    u8 timer;
    u8 dir;
    s16 dx;
    u8 pad492[2];
    s16 dz;
};

extern const s16 gUnk_086149C4[];
u32 Player_IsFlaggedWithGlobalModifier08(u32 x);
void Player_PlayLocalSong(s32, u32);
s32 sub_0823BC54(struct Obj *obj);

static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

void Player_UpdateModifier08Drift(struct Obj *p)
{
    u32 angle;
    const s16 *t;

    if (p->active) {
        if (!Player_IsFlaggedWithGlobalModifier08(p->modifier)) {
            p->played = 0;
            p->timer = 0;
            p->active = 0;
        } else if (p->flags284 & 1) {
            if (p->dir & 1) {
                p->played = 0;
                p->timer = 0;
                p->active = 0;
            } else {
                p->dir = sub_0823BC54(p);
                angle = ((p->dir + 5) & 7) * 32;
                p->dx = scale(gUnk_086149C4[(angle + 64) & 255] * 10);
                p->dz = scale(gUnk_086149C4[angle] * 10);
            }
        } else if (!p->played) {
            if (p->timer) {
                Player_PlayLocalSong(p->slot, 0x335);
                p->played = 1;
            } else {
                p->timer++;
            }
        }
    } else if (Player_IsFlaggedWithGlobalModifier08(p->modifier)) {
        if ((u8)(p->state457 - 1) <= 1 || (p->state457 == 6 && p->v34C >= 0)) {
            p->active = 1;
            p->dir = p->facing;
            angle = ((p->dir + 5) & 7) * 32;
            t = gUnk_086149C4;
            p->dx = scale(t[(angle + 64) & 255] * 10);
            /* reusing the table variable for the result keeps it in r2 */
            t = (const s16 *)scale(t[angle] * 10);
            p->dz = (s32)t;
        }
    }
}
