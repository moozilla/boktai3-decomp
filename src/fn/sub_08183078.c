#include "global.h"

/* tentative: arms the attack collider from facing tables with half power */
#define ActorController_UpdateAttackCollider sub_08183078

struct ActorAnimationLayer {
    u16 unknown[3];
    u16 flags;
};
struct ActorRenderFlags {
    u32 unknown, flags;
};
struct HitBox {
    u32 a, b;
};
struct AttackCollider {
    u8 unknown0[0x14];
    struct HitBox offset;
    struct HitBox size;
    u8 unknown24[0x38 - 0x24];
    u32 flags;
};

extern u32 gUnk_0824EBF4[];
extern const struct HitBox gUnk_0824EC04[];
extern const struct HitBox gUnk_0824EC24[];
extern const struct HitBox gUnk_0824EC44[];
extern const struct HitBox gUnk_0824EC64[];
void Armor_SetAnimationFromStart(u32 a, u32 b);
void Collider_SetAttackPowerAndForce(void *, s32, s32);
void Sound_PlaySongChecked(u32 a);

static inline s32 Half(s32 n)
{
    n <<= 11;
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

void ActorController_UpdateAttackCollider(u8 *p)
{
    u8 *s = p + 0xB40;
    u8 st = *s;

    switch (st) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart((u32)p, (u32)&gUnk_0824EBF4[((p[0xB39] + 0x60) >> 6) & 3]);
            (*s)++;
            p[0xB41] = st;
            ((struct ActorAnimationLayer *)(p + 0x238))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x1E4))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x28C))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x190))->flags |= four;
            {
                u32 one = 1;
                ((struct ActorRenderFlags *)(p + 0x180))->flags &= ~one;
            }
            *(u16 *)(p + 0xB32) &= ~0x20;
            break;
        }
        case 1:
            if (p[0xB43] != 0) {
                *s = 0xFF;
                p[0xB41] = st;
                {
                    u16 *fl = (u16 *)(p + 0x196);
                    u32 old = *fl;
                    u32 set = 4;
                    set |= old;
                    *fl = set;
                }
            }
            break;
    }
    switch (*(u16 *)(p + 0xC8)) {
        case 4: {
            struct AttackCollider *c;
            u8 *facing;
            u32 off;
            *(u16 *)(p + 0x196) &= ~4;
            {
                struct AttackCollider *t = (struct AttackCollider *)(p + 0x190);
                u32 mask = 0x60000;
                t->flags |= mask;
            }
            Collider_SetAttackPowerAndForce(p + 0x190, (u16)Half(*(s16 *)(p + 0xB26)), 50);
            facing = p + 0xB39;
            off = ((*facing + 0x60) >> 6) & 3;
            c = (struct AttackCollider *)(p + 0x190);
            off <<= 3;
            c->size = *(const struct HitBox *)(off + (u32)gUnk_0824EC24);
            c->offset = *(const struct HitBox *)(off + (u32)gUnk_0824EC04);
            {
                u8 f = *facing;
                p[0x1D2] = f;
            }
            break;
        }
        case 5: {
            struct AttackCollider *c;
            u32 off = ((p[0xB39] + 0x60) >> 6) & 3;
            c = (struct AttackCollider *)(p + 0x190);
            off <<= 3;
            c->size = *(const struct HitBox *)(off + (u32)gUnk_0824EC64);
            c->offset = *(const struct HitBox *)(off + (u32)gUnk_0824EC44);
            if (*(u16 *)(p + 0xC0) == 0)
                Sound_PlaySongChecked(0xBF);
            break;
        }
    }
}
