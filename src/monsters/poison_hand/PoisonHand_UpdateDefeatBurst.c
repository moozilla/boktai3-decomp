#include "scene/collision/contact_actor.h"
#include "monsters/poison_hand.h"
#include "engine/math/scene_vec.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/math/monster_particle_random.h"
void PoisonHand_RecordDefeat(void *);
void *SpriteEffectController_Create(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                    u32, s32, s32, u32, s32, u32, u32);
static inline void PoisonHand_CopyPosition(struct SceneVec *dst, struct SceneVec *src)
{
    *dst = *src;
}
s32 PoisonHand_UpdateDefeatBurst(struct PoisonHand *hand)
{
    struct SceneVec position;
    struct ContactPackedVector velocity;
    s32 random, base;
    if (!hand->initialDead) {
        u32 attributes, palette;
        if (hand->age <= 15) {
            PoisonHand_CopyPosition(&position, (struct SceneVec *)&hand->sprite.position);
            position.y += 0x80;
            random = ParticleRandom_ReadDirectSample(gRandomTable);
            base = (u16)hand->spread - 8;
            velocity.x = base + (random & 15);
            velocity.y = 0;
            velocity.z = base + (ParticleRandom_ReadDirectSample(gRandomTable) & 15);
            attributes = 0x8000;
            palette = 0x1c5;
            if (hand->age & 1)
                SpriteEffectController_Create(&position, &velocity, 0x61f9, 0x31f4, 8, 0x200, 9,
                                              palette, 2, 0, 0, 0, hand->effectX, hand->effectY,
                                              attributes, hand->effectZ, 0, 0);
            else
                SpriteEffectController_Create(&position, &velocity, 0x61f9, 0x31f4, 8, 0x400, 9,
                                              palette, 2, 0, 0, 0, hand->effectX, hand->effectY,
                                              attributes, hand->effectZ, 0, 0);
        }
        if (hand->age == 16)
            PoisonHand_RecordDefeat(hand);
    }
    return 0;
}
