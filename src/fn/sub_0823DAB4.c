#include "global.h"
#include "player.h"

#ifdef NONMATCHING
// fuzzy: 97.4%
// 9 differing insns: the second-branch sum goes through a copy in the ROM, and the 0xFF mask is reloaded
extern const s16 gUnk_086149C4[];
void Player_PlayLocalSong(s32, u32);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
void sub_0823D97C(struct Player *, struct SceneVec *, s32, u32, s32);

#define Player_UpdateSweepAction sub_0823DAB4

struct SweepCtx {
    u8 unk0[0x48];
    u8 collider[0x54];
    struct SceneVec dest;   /* 0x9C */
    struct SceneVec origin; /* 0xA4 */
    u16 unkAC;
    u8 unkAE[0xE];
    u16 extentA; /* 0xBC */
    u16 pad;
    u16 extentB; /* 0xC0 */
    u16 pad2;
    u16 pad3;
    u16 timer;  /* 0xC6 */
    u8 heading; /* 0xC8 */
    s8 index;   /* 0xC9 */
};

static inline s32 scale(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

s32 Player_UpdateSweepAction(struct Player *p)
{
    struct SweepCtx *q = (struct SweepCtx *)&p->statModifierContext;
    s32 angle;
    if (p->statModifierContext.contactTimer == 0) {
        if (p->actionMode == 0)
            Player_PlayLocalSong(p->slot, 0x38C);
        else
            Player_PlayLocalSong(p->slot, 0x38B);
    }
    if (++q->timer <= q->extentB) {
        if (p->side != 0)
            angle = q->heading - q->timer * q->extentA + 0x120;
        else {
            u8 *h = &q->heading;
            angle = q->timer * q->extentA + 0x100;
            angle += *h;
            angle -= 0x20;
        }
        angle &= 0xFF;
        q->dest.x = q->origin.x + scale(gUnk_086149C4[(angle + 64) & 255] * 160);
        q->dest.y = q->origin.y;
        q->dest.z = q->origin.z + scale(gUnk_086149C4[angle] * 160);
        Collider_SetPosition(q->collider, &q->dest, 0);
        Collider_Refresh(q->collider);
        if (q->index >= 0) {
            {
                u32 m = 0x80000;
                p->statusFlags |= m;
            }
            if (q->timer & 1)
                sub_0823D97C(p, &q->origin, angle, p->side, q->index);
        }
        return 0;
    }
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0823DAB4);
#endif
