#include "tentacle_spread.h"

// confidence=low: spreads the body's segments out from the actor's position, then releases.

void TentacleBody_UpdateSpreadRelease(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct TentacleBodyView *body = actor->context;
    s32 n, i;
    u32 angle, scale;
    u8 *direction;
    if (TentacleSpread_Take(&actor->field2B2))
        body->spin = 0;
    {
        u16 raw = body->spin;
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
            body->segments[i].offsetX = Div(*(s16 *)((u8 *)actor + 0x64), i + 1);
            body->segments[i].offsetY = 0;
            body->segments[i].offsetZ = Div(*(s16 *)((u8 *)actor + 0x68), i + 1);
            switch (n) {
                case 0:
                    body->segments[i].posX = *(u16 *)((u8 *)actor + 0x5C) -
                                             TentacleSpread_Shift(gUnk_086149C4[(*direction + 64) & 255] * 54, 12);
                    body->segments[i].posZ =
                        *(u16 *)((u8 *)actor + 0x60) - TentacleSpread_Shift(gUnk_086149C4[*direction] * 54, 12);
                    break;
                case 1:
                    body->segments[i].posX = *(u16 *)((u8 *)actor + 0x5C) -
                                             TentacleSpread_Shift(gUnk_086149C4[(*direction + 64) & 255] * 20, 12);
                    body->segments[i].posZ =
                        *(u16 *)((u8 *)actor + 0x60) - TentacleSpread_Shift(gUnk_086149C4[*direction] * 20, 12);
                    break;
                default:
                    body->segments[i].posX = *(u16 *)((u8 *)actor + 0x5C);
                    body->segments[i].posZ = *(u16 *)((u8 *)actor + 0x60);
                    break;
            }
            n++;
        }
    }
    if (frame > *(u16 *)((u8 *)body + 0x8E6)) {
        actor->field2B0 = 1;
        {
            u16 *p = (u16 *)((u8 *)actor + 0x2E8);
            u32 two = 2;
            *p = two;
            {
                u16 *flags = (u16 *)((u8 *)body + 0x8D0);
                s32 mask = two - 5;
                *flags = mask & *flags;
            }
        }
    } else
        sub_080A9DB0(actor, 64, 0xFFFF);
}
