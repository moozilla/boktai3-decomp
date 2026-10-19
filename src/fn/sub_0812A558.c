#include "global.h"
#ifdef NONMATCHING
// fuzzy: 88.5%
// foundry draft (worker best 88.5%); what differs is mostly register allocation and instruction order
#define ActorEffect_OrientFromPositions sub_0812A558
#define ActorEffect_SetDirection        LinkedEffect_SetVariant
#define gRandomIndex                    gUnk_03005308
#define gRandomTable                    gUnk_0203B400
struct EffectVector {
    s16 x, y, z;
};
struct ActorEffectPosition {
    u8 pad[12];
    struct EffectVector position;
};
struct ActorEffectSprite {
    u8 pad[6];
    u16 flags;
    u8 rest[0x54 - 8];
};
struct ActorEffect {
    u8 pad0[0x1c];
    u8 direction[0x58 - 0x1c];
    struct ActorEffectSprite sprites[2];
    u8 pad100[0x176 - 0x100];
    u8 state;
    u8 pad177[3];
    u8 active;
};
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
s32 Math_Atan2(s32, s32);
void LinkedEffect_SetVariant(void *, u32);
static inline void subtract(struct EffectVector *out, struct EffectVector *b,
                            struct EffectVector *a)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
static inline void mark(struct ActorEffectSprite *s)
{
    s->flags |= 4;
}
static inline u32 randomAngle(u32 *current)
{
    u32 index;
    u32 mask;
    index = *current + 1;
    mask = 0x3ff;
    index &= mask;
    *current = index;
    return (u8)gUnk_0203B400[index];
}
void sub_0812A558(struct ActorEffectPosition *a, struct ActorEffectPosition *b,
                  struct ActorEffect *effect)
{
    struct EffectVector delta;
    s32 angle;
    void *direction;
    subtract(&delta, &b->position, &a->position);
    if (delta.x == 0 && delta.z == 0) {
        angle = randomAngle(&gUnk_03005308);
    } else {
        angle = Math_Atan2(delta.x, delta.z);
    }
    direction = effect->direction;
    LinkedEffect_SetVariant(direction, (u8)angle);
    LinkedEffect_SetVariant(direction, (u8)angle);
    mark(&effect->sprites[0]);
    mark(&effect->sprites[1]);
    effect->state = 3;
    effect->active = 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0812A558);
#endif
