// MATCHING_CONCESSION: register-pinning
#include "tentacle_spread.h"
void TentacleBody_BeginDirectedAttack(void);
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
    struct TentacleBodyAnimationView *body = actor->context;
    s32 n, i;
    u32 angle, scale;
    u8 *direction;
    asm("" ::: "r8");
    if (TentacleSpread_Take(&actor->field2B2))
        body->spin = 0;
    {
        u8 raw = body->spin;
        u8 *field;
        if ((s8)body->spin >= 0) {
            field = &actor->body.direction;
            angle = *field;
        } else {
            field = &actor->body.direction;
            angle = (*field + 128) & 255;
        }
        scale = TentacleSpread_Scale(raw);
        direction = field;
    }
    *(u16 *)((u8 *)actor + 0x64) += TentacleSpread_Shift(gUnk_086149C4[(angle + 64) & 255] * scale, 12);
    *(u16 *)((u8 *)actor + 0x68) += TentacleSpread_Shift(gUnk_086149C4[angle] * scale, 12);
    body->spin += 12;
    n = 0;
    for (i = 0; i < body->segmentCount; i++) {
        if (TentacleSpread_Test(&body->segments[i], 2) && !TentacleSpread_Test(&body->segments[i], 5)) {
            body->segments[i].offset.x = Div(*(s16 *)((u8 *)actor + 0x64), i + 1);
            body->segments[i].offset.y = 0;
            body->segments[i].offset.z = Div(*(s16 *)((u8 *)actor + 0x68), i + 1);
            switch (n) {
                case 0:
                    body->segments[i].position.x = *(u16 *)((u8 *)actor + 0x5C) -
                                             TentacleSpread_Shift(gUnk_086149C4[(*direction + 64) & 255] * 54, 12);
                    body->segments[i].position.z =
                        *(u16 *)((u8 *)actor + 0x60) - TentacleSpread_Shift(gUnk_086149C4[*direction] * 54, 12);
                    break;
                case 1:
                    body->segments[i].position.x = *(u16 *)((u8 *)actor + 0x5C) -
                                             TentacleSpread_Shift(gUnk_086149C4[(*direction + 64) & 255] * 20, 12);
                    body->segments[i].position.z =
                        *(u16 *)((u8 *)actor + 0x60) - TentacleSpread_Shift(gUnk_086149C4[*direction] * 20, 12);
                    break;
                default:
                    body->segments[i].position.x = *(u16 *)((u8 *)actor + 0x5C);
                    body->segments[i].position.z = *(u16 *)((u8 *)actor + 0x60);
                    break;
            }
            n++;
        }
    }
    sub_080A9DB0(actor, 64, 0xFFFF);
    if (frame > 60)
        Control(actor, (void *)TentacleBody_BeginDirectedAttack, 36);
}
