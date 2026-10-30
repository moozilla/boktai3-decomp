// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_reset.h"
#define SPIN(b) (*((u8 *)(b) + 0x875))
extern const s16 gUnk_086149C4[];
s32 Div(s32, s32);
void sub_080A9DB0(void *, s32, s32);
void TentacleBody_BeginDirectedAttack(void);
struct TentacleSegment {
    u16 posX;
    u8 pad2[2];
    u16 posZ;
    u8 pad6[0xE9 - 6];
    u8 flags;
    u8 padEA[0x100 - 0xEA];
    u16 offsetX, offsetY, offsetZ;
    u8 pad106[0x22];
};
struct TentacleBodyView {
    u8 pad0[0x3F0];
    struct TentacleSegment segments[3];
    u8 pad768[0x8DA - 0x768];
    s16 segmentCount;
};
static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u8 Test(struct TentacleSegment *s, u32 m)
{
    if (s->flags & m)
        return 1;
    return 0;
}
static inline s32 Shift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}
static inline void Control(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
{
    u32 zero;
    a->field2B2 = 1;
    zero = 0;
    a->field2B0 = zero;
    a->field2B5 = kind;
    a->target = cb;
    a->field2C8 = zero;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = zero;
}
// confidence=low: spreads the body's segments out from the actor's position, then attacks.

void TentacleBody_UpdateChargeSpread(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct TentacleBodyView *body = actor->context;
    s32 n, i;
    u32 angle, scale;
    u8 *direction;
    asm("" ::: "r8");
    if (Take(&actor->field2B2))
        SPIN(body) = 0;
    {
        u8 raw = SPIN(body);
        u8 *field;
        if ((s8)SPIN(body) >= 0) {
            field = &actor->body.direction;
            angle = *field;
        } else {
            field = &actor->body.direction;
            angle = (*field + 128) & 255;
        }
        {
            u32 mask = 0x7F;
            asm("" : "+r"(mask));
            scale = (mask & raw) >> 4;
        }
        direction = field;
    }
    *(u16 *)((u8 *)actor + 0x64) += Shift(gUnk_086149C4[(angle + 64) & 255] * scale, 12);
    *(u16 *)((u8 *)actor + 0x68) += Shift(gUnk_086149C4[angle] * scale, 12);
    SPIN(body) += 12;
    n = 0;
    for (i = 0; i < body->segmentCount; i++) {
        if (Test(&body->segments[i], 2) && !Test(&body->segments[i], 5)) {
            body->segments[i].offsetX = Div(*(s16 *)((u8 *)actor + 0x64), i + 1);
            body->segments[i].offsetY = 0;
            body->segments[i].offsetZ = Div(*(s16 *)((u8 *)actor + 0x68), i + 1);
            switch (n) {
                case 0:
                    body->segments[i].posX = *(u16 *)((u8 *)actor + 0x5C) -
                                             Shift(gUnk_086149C4[(*direction + 64) & 255] * 54, 12);
                    body->segments[i].posZ =
                        *(u16 *)((u8 *)actor + 0x60) - Shift(gUnk_086149C4[*direction] * 54, 12);
                    break;
                case 1:
                    body->segments[i].posX = *(u16 *)((u8 *)actor + 0x5C) -
                                             Shift(gUnk_086149C4[(*direction + 64) & 255] * 20, 12);
                    body->segments[i].posZ =
                        *(u16 *)((u8 *)actor + 0x60) - Shift(gUnk_086149C4[*direction] * 20, 12);
                    break;
                default:
                    body->segments[i].posX = *(u16 *)((u8 *)actor + 0x5C);
                    body->segments[i].posZ = *(u16 *)((u8 *)actor + 0x60);
                    break;
            }
            n++;
        }
    }
    sub_080A9DB0(actor, 64, 0xFFFF);
    if (frame > 60)
        Control(actor, (void *)TentacleBody_BeginDirectedAttack, 36);
}
