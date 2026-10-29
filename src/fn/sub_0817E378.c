#include "global.h"
#include "engine_globals.h"
#include "ram.h"
struct SceneEventActor {
    u8 unknown00[0x5c];
    s16 velocityX;
    u8 unknown5E[2];
    s16 velocityZ;
    u8 unknown62[0x9c - 0x62];
    u32 flags;
    u8 unknownA0[2];
    u8 phase;
    u8 unknownA3;
    u32 timer;
    u8 unknownA8[7];
    u8 pending;
    u8 unknownB0;
    u8 restart;
    u8 finished;
    u8 unknownB3[0x104 - 0xb3];
    u32 callback;
    u8 unknown108[0x2ac - 0x108];
    u8 animationState[0x20];
    u8 animation[0x471 - 0x2cc];
    u8 direction;
    u8 speed;
    u8 unknown473[0x510 - 0x473];
    s8 randomOffset;
    u8 jitterTicks;
};
extern const s16 gUnk_086149C4[];
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Callback_InvokeActor(void *, u32);
static inline u16 RandomSample(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)(gRandomIndex * 2 + (u32)gRandomTable);
}
void sub_0817E378(struct SceneEventActor *p)
{
    if (p->restart) {
        p->restart = 0;
        SceneActors_StartDirectionalAnimation(p, p->animation, p->animationState, 0x114, 1, 1, 4);
        p->jitterTicks = 6;
    }
    if (p->jitterTicks) {
        s32 random = RandomSample();
        p->randomOffset = random >> 8;
        p->jitterTicks--;
        {
            u32 flags = 0x20000;
            p->flags |= flags;
        }
    }
    {
        u32 flags = 0x400005;
        p->flags |= flags;
    }
    p->flags |= 0x40;
    Callback_InvokeActor(p, p->callback);
}

static inline s32 ScaleMovement(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

void sub_0817E424(struct SceneEventActor *p)
{
    struct SceneEventActor *actor = p;
    if (p->restart) {
        p->restart = 0;
        SceneActors_StartDirectionalAnimation(p, p->animation, p->animationState, 0x11a, 1, 1, 4);
        p->flags |= 1;
    }
    switch (p->phase) {
        case 0:
            if (p->speed) {
                u32 angle = p->direction;
                p->velocityX = ScaleMovement(gUnk_086149C4[(angle + 64) & 255] * p->speed);
                p->velocityZ = ScaleMovement(gUnk_086149C4[angle] * actor->speed);
                actor->speed = (actor->speed * 3) >> 3;
                {
                    u32 one = 1;
                    p->flags |= one;
                }
            } else {
                p->speed = 0;
                SceneActors_StartDirectionalAnimation(p, p->animation, p->animationState, 0x11c, 1,
                                                      1, 1);
                if (p->finished)
                    SceneActors_StartDirectionalAnimation(p, p->animation, p->animationState, 0x11e,
                                                          1, 1, 1);
            }
            /* Fall through while the movement animation settles. */
        case 1:
            ++p->timer;
            if (p->finished || p->timer > 30) {
                SceneActors_StartEightDirectionAnimation(p, actor->animation, actor->animationState,
                                                         0xf4, 1, 1, 1);
                {
                    u32 next = 2;
                    u32 zero = 0;
                    p->phase = next;
                    p->timer = zero;
                }
            }
            break;
        case 2:
            p->pending = 1;
            break;
    }
}
