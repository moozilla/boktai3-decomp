#include "engine/math/math_shift.h"
#include "monsters/poison_hand.h"
#include "engine/math/scene_vec.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/math/monster_particle_random.h"
void PoisonHand_RecordDefeat(void *);
void *SpriteEffectController_Create(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                    u32, s32, s32, u32, s32, u32, u32);
struct BurstVelocity {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
static inline void PoisonHand_CopyPosition(struct SceneVec *dst, struct SceneVec *src)
{
    *dst = *src;
}

extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Anim_SetDurationScale(void *, u16);
void PoisonHand_WaitForPlayerApproach(void);
struct ContactPosition *Player_GetEffectivePosition(s32);

// confidence=low: maintains the approach-triggered emergence animation and particle emission.
static inline void Polar(struct ContactPosition *v, u8 angle, s32 magnitude)
{
    v->x = Math_ShiftTowardZero(gUnk_086149C4[(angle + 64) & 255] * magnitude, 12);
    v->y = 0;
    v->z = Math_ShiftTowardZero(gUnk_086149C4[angle] * magnitude, 12);
}
static inline s32 Distance(struct ContactPosition *a, struct ContactPosition *b)
{
    s32 x = b->x - a->x, z = b->z - a->z;
    return x * x + z * z;
}
s32 PoisonHand_UpdateEmergence(struct PoisonHand *hand)
{
    struct SceneVec position;
    struct ContactPosition velocity;
    s32 angle;
    s32 jitter;
    u32 attributes;
    u32 palette;
    PoisonHand_CopyPosition(&position, (struct SceneVec *)&hand->sprite.position);
    position.y += 0x80;
    angle = ParticleRandom_ReadDirectSample(gRandomTable) & 255;
    jitter = ParticleRandom_ReadDirectSample(gRandomTable);
    Polar(&velocity, angle, hand->spread + (jitter & 15) - 8);
    attributes = 0x8000;
    palette = 0x1c5;
    if (Mod(hand->countdown, 10) == 0 && hand->countdown > 90) {
        if (Mod(hand->countdown, 20) == 0)
            SpriteEffectController_Create(&position, &velocity, 0x61f9, 0x31f4, 8, 0x200, 9,
                                          palette, 2, 0, 0, 0, hand->effectX, hand->effectY,
                                          attributes, hand->effectZ, 0, 0);
        else
            SpriteEffectController_Create(&position, &velocity, 0x61f9, 0x31f4, 8, 0x400, 9,
                                          palette, 2, 0, 0, 0, hand->effectX, hand->effectY,
                                          attributes, hand->effectZ, 0, 0);
    }
    if (hand->countdown > 0) {
        hand->countdown--;
        if (hand->countdown == 30) {
            if (hand->attackDistanceSquared <
                Distance(&hand->sprite.position, Player_GetEffectivePosition(0))) {
                Anim_RestartSequence(&hand->sequence, hand->animationResource, 0x18, 0, 12);
                Anim_SetDurationScale(&hand->sequence, 0x20);
            } else {
                hand->countdown = 180;
                Sound_PlaySong(0x51c);
            }
        }
    } else {
        Anim_RestartSequence(&hand->sequence, hand->animationResource, 0x17, 0, 0);
        hand->update = PoisonHand_WaitForPlayerApproach;
    }
    return 0;
}
