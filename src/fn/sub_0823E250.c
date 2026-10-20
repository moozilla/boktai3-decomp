#include "global.h"
#include "player.h"

extern const s16 gUnk_086149C4[];
void Player_PlayLocalSong(s32, u32);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
void sub_0823E124(struct Player *, struct SceneVec *, u32, s32);

#define Player_UpdateSpinAction sub_0823E250

struct SpinCtx {
    u8 unk0[0x48];
    u8 collider[0x54];
    struct SceneVec dest;   /* 0x9C */
    struct SceneVec origin; /* 0xA4 */
    u8 unkAC[0x10];
    u16 speed;     /* 0xBC */
    u16 unkBE;
    u16 phase1;    /* 0xC0 */
    u16 phase2;    /* 0xC2 */
    u16 phase3;    /* 0xC4 */
    u16 timer;     /* 0xC6 */
    u8 heading;    /* 0xC8 */
    s8 index;      /* 0xC9 */
};

static inline s32 scale(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

#define PLACE(radius)                                                                         \
    q->dest.x = q->origin.x + scale(gUnk_086149C4[(q->heading + 64) & 255] * (radius));      \
    q->dest.y = q->origin.y;                                                                  \
    q->dest.z = q->origin.z + scale(gUnk_086149C4[q->heading] * (radius));                   \
    Collider_SetPosition(q->collider, &q->dest, 0);                                           \
    Collider_Refresh(q->collider);

s32 Player_UpdateSpinAction(struct Player *p)
{
    struct SpinCtx *q = (struct SpinCtx *)&p->statModifierContext;
    if (p->statModifierContext.contactTimer == 0) {
        if (p->actionMode == 0)
            Player_PlayLocalSong(p->slot, 0x38F);
        else
            Player_PlayLocalSong(p->slot, 0x38D);
    }
    if (q->timer < q->phase1) {
        s32 radius = q->speed * q->timer + 0xCC;
        PLACE(radius)
        if (q->index >= 0) {
            { u32 m = 0x80000; p->statusFlags |= m; }
            sub_0823E124(p, &q->dest, q->heading, q->index);
        }
    } else if (q->timer < q->phase1 + q->phase2) {
        s32 radius = q->speed * q->phase1 + 0xCC;
        PLACE(radius)
        if (q->index >= 0) {
            { u32 m = 0x80000; p->statusFlags |= m; }
            if (!(q->timer & 1))
                sub_0823E124(p, &q->dest, q->heading, q->index);
        }
    }
    if (++q->timer >= q->phase1 + q->phase2 + q->phase3) {
        q->timer = 0;
        return 1;
    }
    return 0;
}
