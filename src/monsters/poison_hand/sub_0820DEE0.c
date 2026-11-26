#include "engine/math/math_shift.h"
#include "monsters/poison_hand.h"
#include "engine/math/scene_vec.h"
#include "engine/engine.h"
#include "engine/ram.h"
void PoisonHand_RecordDefeat(void *);
void *SpriteEffectController_Create(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                    u32, s32, s32, u32, s32, u32, u32);
struct BurstVelocity {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
static inline s32 PoisonHand_NextSpread(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)table);
}
static inline void PoisonHand_CopyPosition(struct SceneVec *dst, struct SceneVec *src)
{
    *dst = *src;
}
static inline void PoisonHand_RaiseOrigin(struct SceneVec *p)
{
    p->y += 0x80;
}

extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Anim_SetDurationScale(void *, u16);
void PoisonHand_WaitForPlayerApproach(void);
struct SceneVec *Player_GetEffectivePosition(s32);

// confidence=low: maintains the approach-triggered emergence animation and particle emission.
struct Velocity {
    s16 x, y, z, pad;
};
static inline void Polar(struct Velocity *v, u8 angle, s32 magnitude)
{
    v->x = Math_ShiftTowardZero(gUnk_086149C4[(angle + 64) & 255] * magnitude, 12);
    v->y = 0;
    v->z = Math_ShiftTowardZero(gUnk_086149C4[angle] * magnitude, 12);
}
static inline s32 Distance(struct SceneVec *a, struct SceneVec *b)
{
    s32 x = b->x - a->x, z = b->z - a->z;
    return x * x + z * z;
}
s32 PoisonHand_UpdateEmergence(struct PoisonHand *input)
{
    struct PoisonHand *p = input;
    struct SceneVec position;
    struct Velocity velocity;
    s32 angle;
    s32 magnitude;
    s32 jitter;
    u32 attributes;
    u32 palette;
    PoisonHand_CopyPosition(&position, (struct SceneVec *)&p->sprite.position);
    PoisonHand_RaiseOrigin(&position);
    angle = PoisonHand_NextSpread(gRandomTable) & 255;
    jitter = PoisonHand_NextSpread(gRandomTable);
    magnitude = p->spread;
    jitter &= 15;
    Polar(&velocity, angle, magnitude + jitter - 8);
    attributes = 0x8000;
    palette = 0x1c5;
    if (Mod(p->countdown, 10) == 0 && p->countdown > 90) {
        if (Mod(p->countdown, 20) == 0)
            SpriteEffectController_Create(&position, &velocity, 0x61f9, 0x31f4, 8, 0x200, 9,
                                          palette, 2, 0, 0, 0, p->effectX, p->effectY, attributes,
                                          p->effectZ, 0, 0);
        else
            SpriteEffectController_Create(&position, &velocity, 0x61f9, 0x31f4, 8, 0x400, 9,
                                          palette, 2, 0, 0, 0, p->effectX, p->effectY, attributes,
                                          p->effectZ, 0, 0);
    }
    if (p->countdown > 0) {
        p->countdown--;
        if (p->countdown == 30) {
            s32 *range = &p->attackDistanceSquared;
            struct SceneVec *player = Player_GetEffectivePosition(0);
            if (*range < Distance((struct SceneVec *)&p->sprite.position, player)) {
                Anim_RestartSequence(&p->sequence, p->animationResource, 0x18, 0, 12);
                Anim_SetDurationScale(&p->sequence, 0x20);
            } else {
                p->countdown = 180;
                Sound_PlaySong(0x51c);
            }
        }
    } else {
        Anim_RestartSequence(&p->sequence, p->animationResource, 0x17, 0, 0);
        p->update = PoisonHand_WaitForPlayerApproach;
    }
    return 0;
}
