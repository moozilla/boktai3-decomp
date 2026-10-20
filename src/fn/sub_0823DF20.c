#include "global.h"
#include "player.h"

extern const s16 gUnk_086149C4[];
u32 Player_GetConfiguredActionPower(struct Player *);
u32 Player_CollectCallbackFlags(struct Player *);
void Player_ConsumeActionResource(struct Player *);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
s32 sub_0823DE6C(void *, u32, u32);

struct SlashCtx {
    u8 unk0[0x48];
    u8 collider[0x42];
    u8 unk8A;
    u8 pad8B[0x11];
    struct SceneVec dest;   /* 0x9C */
    struct SceneVec origin; /* 0xA4 */
    u8 unkAC[0xC];
    u16 attackA; /* 0xB8 */
    u16 attackB; /* 0xBA */
    u16 w[5];    /* 0xBC..0xC4 */
    u16 timer;   /* 0xC6 */
    u8 heading;  /* 0xC8 */
    s8 index;    /* 0xC9 */
};

static inline s32 scale(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

void Player_BeginSlashAction(struct Player *p)
{
    struct SlashCtx *q = (struct SlashCtx *)&p->statModifierContext;
    u32 power, flags;
    s32 kind;
    u16 attackA;
    *(struct SceneVec *)&q->origin = p->position;
    q->origin.y += 0x96;
    q->heading = ((p->facing + 5) & 7) << 5;
    q->dest.x = q->origin.x + scale(gUnk_086149C4[(q->heading + 64) & 255] * 0xCC);
    q->dest.y = q->origin.y;
    q->dest.z = q->origin.z + scale(gUnk_086149C4[q->heading] * 0xCC);
    power = Player_GetConfiguredActionPower(p);
    flags = Player_CollectCallbackFlags(p);
    if (p->actionMode == 1)
        flags |= 0x40000;
    attackA = q->attackA;
    kind = sub_0823DE6C(&q->origin, q->heading, 0xE0);
    if (kind != 0) {
        if ((q->index == 2 || q->index == 4) && kind == 2) {
            q->w[1] = 0x1B;
            q->w[0] = 0x38;
            q->w[2] = 0;
            q->w[3] = 3;
            q->w[4] = 6;
        } else {
            q->w[1] = 0x1A;
            q->w[0] = 0x38;
            q->w[2] = 0;
            q->w[3] = 3;
            q->w[4] = 0;
        }
    } else if (sub_0823DE6C(&q->origin, q->heading, 0x150) != 0) {
        q->w[1] = 0x1B;
        q->w[0] = 0x38;
        q->w[2] = 2;
        q->w[3] = 7;
        q->w[4] = kind;
    } else {
        q->w[1] = 0x1C;
        q->w[0] = 0x38;
        q->w[2] = 4;
        q->w[3] = 7;
        q->w[4] = 5;
    }
    if (q->index >= 0) {
        flags |= 1 << q->index;
        Player_ConsumeActionResource(p);
    }
    Collider_SetAttackProperties(q->collider, power, attackA, 2, flags, q->attackB);
    q->unk8A = q->heading;
    q->timer = 0;
}
