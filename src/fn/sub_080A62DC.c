// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_reset.h"
#define SPIN(b) (*((u8 *)(b) + 0x875))
extern const s16 gUnk_086149C4[];
s32 Div(s32, s32);
void sub_080A9DB0(void *, s32, s32);
struct TentacleSegment {
    u8 unknown0[0x1C];
    u16 posX;
    u8 pad2[2];
    u16 posZ;
    u8 pad6[0xE9 - 6];
    u8 flags;
    u8 padEA[0x100 - 0xEA];
    u16 offsetX, offsetY, offsetZ;
    u8 pad106[6];
};
struct TentacleBodyView {
    u8 pad0[0x3D4];
    struct TentacleSegment segments[3];
    u8 pad768[0x8DA - 0x74C];
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
void TentacleBody_GetDirectionFrame(u16, u32 *, u32 *);
void TentacleBody_RestartSegmentAnimations(void *, u32, u32, u32);
void TentacleBody_UpdateSpreadRelease(void);
static inline void SetBodyFlag(struct TentacleBodyView *body, u32 input)
{
    u32 mask = input;
    u16 *f;
    asm("" : "+r"(mask));
    f = (void *)((u8 *)body + 0x8D0);
    *f = mask | *f;
}
// confidence=low: winds up a segment-spread attack while backing away from the target.
void TentacleBody_UpdateSpreadWindup(struct SceneActorRegistryResetView *actor)
{
    struct TentacleBodyView *body = actor->context;
    s32 n, i, threshold, speed;
    u32 zero;
    struct {
        u32 zero, variant, flags;
    } params;
    if (Take(&actor->field2B2)) {
        {
            u32 mask = 0x100000;
            actor->flags2D0 |= mask;
        }
        TentacleBody_GetDirectionFrame(actor->body.direction, &params.variant, &params.flags);
        TentacleBody_RestartSegmentAnimations(body, 18, params.variant, params.flags);
        SetBodyFlag(body, 2);
    }
    {
        struct ContactPosition *destination = &actor->secondaryMotion;
        zero = 0;
        params.zero = zero;
        {
            u32 *source = &params.zero;
            asm("" : "+r"(source));
            CpuSet(source, destination, 0x05000002);
        }
    }
    {
        u32 count = ((u8 *)body)[0x8EA];
        threshold = 200;
        if (count > 3)
            threshold = 300;
    }
    if ((s16)actor->field2EA >= threshold) {
        void *target = TentacleBody_UpdateSpreadRelease;
        u32 kind = 39;
        actor->field2B2 = 1;
        actor->field2B0 = zero;
        actor->field2B5 = kind;
        actor->target = target;
        actor->field2C8 = zero;
        {
            u32 *f = &actor->flags114;
            s32 mask = -2;
            *f &= mask;
        }
        actor->word104 = zero;
    } else {
        u32 angle;
        if ((s32)actor->counter2AC <= 29) {
            speed = 1;
            actor->field2EA += 10;
        } else {
            speed = 2;
            actor->field2EA += 40;
        }
        angle = (actor->body.direction + 128) & 255;
        actor->body.motion.x += Shift(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
        actor->body.motion.z += Shift(gUnk_086149C4[angle] * speed, 12);
        sub_080A9DB0(actor, 14, 12000);
        n = 0;
        for (i = 0; i < body->segmentCount; i++) {
            if (Test(&body->segments[i], 2) && !Test(&body->segments[i], 5)) {
                switch (n) {
                    case 0:
                        body->segments[i].posX =
                            *(u16 *)((u8 *)actor + 0x5C) -
                            Shift(gUnk_086149C4[(actor->body.direction + 64) & 255] * 54, 12);
                        body->segments[i].posZ = *(u16 *)((u8 *)actor + 0x60) -
                                                 Shift(gUnk_086149C4[actor->body.direction] * 54, 12);
                        break;
                    case 1:
                        body->segments[i].posX =
                            *(u16 *)((u8 *)actor + 0x5C) -
                            Shift(gUnk_086149C4[(actor->body.direction + 64) & 255] * 20, 12);
                        body->segments[i].posZ = *(u16 *)((u8 *)actor + 0x60) -
                                                 Shift(gUnk_086149C4[actor->body.direction] * 20, 12);
                        break;
                    default:
                        body->segments[i].posX = *(u16 *)((u8 *)actor + 0x5C);
                        body->segments[i].posZ = *(u16 *)((u8 *)actor + 0x60);
                        break;
                }
                n++;
            }
        }
    }
}
