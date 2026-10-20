#include "global.h"
#include "player.h"

extern const s16 gUnk_086149C4[];
void Player_PlayLocalSong(s32, u32);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
void sub_0823E810(struct Player *, struct SceneVec *, u32, u32, s32);

struct ThrowCtx {
    u8 unk0[0x48];
    u8 collider[0x54];
    struct SceneVec dest;   /* 0x9C */
    struct SceneVec origin; /* 0xA4 */
    u8 unkAC[0x10];
    u16 speed; /* 0xBC */
    u16 unkBE;
    u16 phase1; /* 0xC0 */
    u16 phase2; /* 0xC2 */
    u16 phase3; /* 0xC4 */
    u16 timer;  /* 0xC6 */
    u8 heading; /* 0xC8 */
    s8 index;   /* 0xC9 */
};

struct Vec16 {
    u32 x : 16, y : 16, z : 16;
};

static inline s32 scale(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

#define Player_UpdateThrowArc sub_0823E998
s32 Player_UpdateThrowArc(struct Player *p)
{
    struct ThrowCtx *q = (struct ThrowCtx *)&p->statModifierContext;
    if (++q->timer <= q->phase3)
        return 0;
    if (q->timer <= q->phase3 + q->phase1) {
        u32 angle;
        s32 t;
        if (q->timer == q->phase3 + q->phase1) {
            if (p->actionMode == 0)
                Player_PlayLocalSong(p->slot, 0x390);
            else
                Player_PlayLocalSong(p->slot, 0x38E);
        }
        angle = (0x150 - (q->timer - q->phase3) * q->speed) & 255;
        q->dest.y = q->origin.y + scale(gUnk_086149C4[angle & 255] * 250);
        t = scale(gUnk_086149C4[(angle + 64) & 255] * 250);
        q->dest.x = q->origin.x + scale(gUnk_086149C4[(q->heading + 64) & 255] * t);
        q->dest.z = q->origin.z + scale(gUnk_086149C4[q->heading] * t);
        if (q->timer > q->phase3 + 2 && q->index >= 0)
            sub_0823E810(p, &q->dest, angle, q->heading, q->index);
        if (q->timer > q->phase3 + q->phase1 - 2) {
            struct Vec16 pos;
            s32 radius = 0xAA;
            pos.x = q->origin.x + scale(gUnk_086149C4[(q->heading + 64) & 255] * radius);
            pos.y = (q->phase3 + q->phase1 - q->timer + 1) * 90 + q->origin.y - 0x104;
            pos.z = q->origin.z + scale(gUnk_086149C4[q->heading] * radius);
            if (q->index >= 0) {
                u32 m = 0x80000;
                p->statusFlags |= m;
            }
            Collider_SetPosition((u8 *)p + 0xB8C, &pos, 0);
            Collider_Refresh(q->collider);
        }
        return 0;
    }
    if (q->timer <= q->phase3 + q->phase1 + q->phase2) {
        if (q->index >= 0) {
            u32 m = 0x80000;
            p->statusFlags |= m;
        }
        Collider_Refresh((u8 *)p + 0xB8C);
        return 0;
    }
    q->timer = 0;
    return 1;
}
